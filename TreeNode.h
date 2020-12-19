#pragma once
#define NDEBUG

#include "board.h"
#include <memory>
#include <climits>
#include <vector>
#include <utility>
#include <shared_mutex>

class MonteCarloTree;

class TreeNode {
private:
	// TreeNode* child;
	std::unique_ptr<TreeNode[]> child;
	PIECE color;
	Pair move;
	std::size_t child_size;

	mutable std::shared_mutex m_mutex;

public:
	int total_count;
	int win_count;
	// double means;

public:		
	friend class MonteCarloTree;

	TreeNode() : child(nullptr), child_size(0), total_count(0), win_count(0) {}
	~TreeNode() {}
	/*** For root parallelization ***/
	
	Pair get_move(){
		return move;
	}
	std::unique_ptr<TreeNode[]> get_child(){
		return std::move(child);
	}
	std::size_t get_child_size(){
		return child_size;
	}
	/*** For root parallelization ***/
	
	void init_TreeNode (const Pair &m, const PIECE &piece) {
		this->color = piece;
		this->move = m;
	}


	void addCount(int t_count, int w_count) {
		std::unique_lock<std::shared_mutex> wLock(m_mutex);
		total_count += t_count;
		win_count += w_count;
	}

	void addresultLock (const WIN_STATE &result) {
		// result is draw
		int t_count = 1, w_count = 0;

		if ( (result == BLACK_WIN && color==BLACK) || (result == WHITE_WIN && color==WHITE) ) {
			w_count++;
		}
		/*----- Critical Section -----*/
		addCount(t_count, w_count);
		/*----- Critical Section -----*/
	}

	void addresult (const WIN_STATE &result) {
		if (result == DRAW)
			total_count++;
		else if ( (result == BLACK_WIN && color==BLACK) || (result == WHITE_WIN && color==WHITE) ) {
			total_count++;
			win_count++;
		}
		else
			total_count++;
	}

	void expand(board &b) {
		
		const PIECE& c = b.take_turn();
		std::vector<Pair> mv = b.get_available_move(c);

		child_size = mv.size();

		if (child_size == 0)
			return;
		
		child = std::make_unique<TreeNode[]>(child_size);
		int idx = 0;
		 
		for (auto &m : mv) {
			child[idx].init_TreeNode(m, c);
			idx++;
		}
	}

	void expandLock(board &b) {
		const PIECE& c = b.take_turn();
		std::vector<Pair> mv = b.get_available_move(c);

		child_size = mv.size();

		if (child_size == 0)
			return;
		

		/*----- Critical Section -----*/
		std::unique_lock<std::shared_mutex> wLock(m_mutex);
		// check if other thread has expanded or not
		if (child != nullptr) {
			return;
		}

		child = std::make_unique<TreeNode[]>(child_size);
		int idx = 0;
		 
		for (auto &m : mv) {
			child[idx].init_TreeNode(m, c);
			idx++;
		}
		/*----- Critical Section -----*/
	}

	Pair best_child() {
		std::size_t best_child_idx = -1;
		double most_visit = INT_MIN;
		double tmp;

		for (std::size_t i = 0 ; i < child_size; ++i) {
			tmp = child[i].total_count;
			if (tmp > most_visit) {
				most_visit = tmp;
				best_child_idx = i;
			}
		}
		//printf("max: %lf\n", most_visit);
		// no more step can go
		if (child_size == 0) {
			return {};
		} 

		auto best_child { this->child.get() + best_child_idx };
		
		return std::move( best_child->move );
	}
	void showchild() {
		std::cerr << std::setw(6) <<"from:";
		std::cerr << std::setw(10) << "to:";
		std::cerr << "  win rate     total_count\n";
		for(std::size_t i=0; i < child_size; ++i) {
			if (child[i].total_count >= 0) {
				std::cerr << "(" << child[i].move.prev/6 << ", "<< child[i].move.prev%6  << ") to (" << child[i].move.next/6 << ", " <<child[i].move.next%6 << ")  " 
				<< child[i].win_count/child[i].total_count << ' ' << child[i].total_count << ' ';
				std::cerr << "\n\n";
			}
		}
	}


};