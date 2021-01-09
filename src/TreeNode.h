#pragma once

#include "board.h"

#include <memory>
#include <climits>
#include <vector>
#include <utility>
#include <shared_mutex>
#include <atomic>

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
	std::atomic<int> total_count;
	std::atomic<int> win_count;
	std::atomic<int> virtual_loss;
	// double means;

public:		
	friend class MonteCarloTree;

	TreeNode() : child(nullptr), child_size(0), total_count(0), win_count(0), virtual_loss(0) {}
	
	~TreeNode() {}

	TreeNode& operator = (const TreeNode &node) {
		this->child_size = node.child_size;
		this->child = std::make_unique<TreeNode[]>(this->child_size);

		std::copy(node.child.get(), node.child.get() + node.child_size, child.get() );

		this->color = node.color;
		this->move = node.move;


		this->total_count.store(node.total_count.load());
		this->win_count.store(node.win_count.load());
		this->virtual_loss.store(node.virtual_loss.load());
		return *this;
	}

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

	void addresult (const WIN_STATE &result) {
		if (result == DRAW)
			total_count++;
		else if ( (result == BLACK_WIN && color==BLACK) || (result == WHITE_WIN && color==WHITE) ) {
			total_count++;
			win_count++;
		}
		else
			total_count++;
		virtual_loss--;
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
		
		

		/*----- Critical Section -----*/
		std::unique_lock<std::shared_mutex> wLock(m_mutex);

		// check if other thread has expanded the leaf or not
		if (child != nullptr) {
			return;
		}

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
		std::cerr << std::setw(6) <<"from:\t ";
		std::cerr << std::setw(10) << "to:\t\t";
		std::cerr << "win rate    ";
		std::cerr << "total_count\n";
		for(std::size_t i=0; i < child_size; ++i) {
			if (child[i].total_count > 0) {
				std::cerr << "(" << child[i].move.prev/6 << ", "<< child[i].move.prev%6  << ")\t\t(" << child[i].move.next/6 << ", " <<child[i].move.next%6 << ")\t\t" 
				<< std::fixed << std::setprecision(5) << child[i].win_count/ (double) child[i].total_count << "     " << child[i].total_count << ' ';
				std::cerr << "\n\n";
			}
		}
	}


};