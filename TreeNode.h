#pragma once
#define NDEBUG

#include "board.h"
#include <memory>
#include <climits>
class MonteCarloTree;

class TreeNode {
private:
	// TreeNode* child;
	std::unique_ptr<TreeNode[]> child;
	PIECE color;
	Pair move;
	std::size_t child_size;
public:
	double total_count;
	double win_count;
	// double means;

public:		
	friend class MonteCarloTree;

	TreeNode() : child(nullptr), child_size(0), total_count(0), win_count(0) {}
	~TreeNode() {}
	
	void init_TreeNode (const Pair &m, const PIECE &piece) {
		this->color = piece;
		this->move = m;
	}
	
	void addresult (const WIN_STATE &result) {
		// result is draw
		if (result == DRAW)
			total_count++;
		else if ( (result == BLACK_WIN && color==BLACK) || (result == WHITE_WIN && color==WHITE) ) {
			total_count++;
			win_count++;
			//means = (means*total_count+1.00)/(total_count+1);
		}
		else
			total_count++;
	//	else
			//means = (means*total_count)/(total_count+1);

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