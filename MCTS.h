#pragma once

#include "TreeNode.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <climits>
#include <omp.h>
// #define THREAD_NUM 4

class MonteCarloTree {
public:
	std::unique_ptr<TreeNode> root;
	// std::vector<TreeNode*> path;
	board root_board;
	
	std::random_device rd;
	std::default_random_engine eng;
	double c_virtual_loss = 5;
	static constexpr double explore_parameter = sqrt(2.0);

	MonteCarloTree() : root(), root_board(), eng(rd()) {}
	TreeNode* UCB (TreeNode* n)  {

		if(n->child_size == 0) return nullptr;
		
		constexpr double eps = 1e-3;
		double max_score = INT_MIN;
		std::size_t same_score[100]{};
		std::size_t idx = 0;

		for (std::size_t i = 0; i < n->child_size; ++i) {
			TreeNode* childNode = n->child.get() + i;
			
			int child_win_count = childNode->win_count.load();
			int child_total_count = childNode->total_count.load();
			
			double virtual_loss = c_virtual_loss * n->virtual_loss.load();

			const double exploit { child_win_count / (double)( child_total_count + 1.0) };
			const double explore { sqrt( log( n->total_count.load() ) / (double)( child_total_count + 1.0) ) };
			
			// without virtual loss
			const double score { exploit + explore_parameter * explore };

			// with virtual loss
			// const double score { exploit + explore_parameter * explore - virtual_loss };
			
			if ( (score <= (max_score + eps) ) && (score >= (max_score - eps) ) ) {
				same_score[idx] = i;
				idx++;
			}
			else if (score > max_score) {
				max_score = score;
				same_score[0] = i;
				idx = 1;
			}
		}
		
		std::shuffle(std::begin(same_score), std::begin(same_score) + idx, eng);
		std::size_t best_idx = same_score[0];
		
		// add virtual loss
		n->virtual_loss++;

		return (n->child.get() + best_idx); 
	}

	void select(board &b, std::vector<TreeNode*> &path) {

		TreeNode* current { root.get() };
	
		// path.clear();
		path.push_back(current);

		while (current->child != nullptr && current->child_size != 0) {
			// move to next children node
			current = UCB(current);
			path.push_back(current);

			b.move(current->move.prev, current->move.next, current->color);
		}
	}


	bool GreedyRollout(board &b, PIECE color) {
		std::vector<Pair> ea { b.find_piece(color, EAT) };
		std::vector<Pair> mv { b.find_piece(color, MOVE) };
			
		if (!ea.empty()) {
			std::shuffle(ea.begin(), ea.end(), eng);
			b.move(ea[0].prev, ea[0].next, color);
		}
		else if (!mv.empty()) {
			std::shuffle(mv.begin(), mv.end(), eng);
			b.move(mv[0].prev, mv[0].next, color);
		} else {
			return false;
		}
		return true;
	}
	bool RandomRollout(board &b, PIECE color) {
		std::vector<Pair> mvs { b.get_available_move(color) };

		if (!mvs.empty()) {
			std::shuffle(mvs.begin(), mvs.end(), eng);
			b.move(mvs[0].prev, mvs[0].next, color);
		} else {
			return false;
		}
		return true;

	}

	WIN_STATE simulate(board b) {

		std::size_t count_step = 0;
		
		constexpr std::size_t limit_step = 500;
		while (true) {
			count_step++;
			// Game draw if exceed the limit step
			if (count_step > limit_step) {
				return b.compare_piece();
			}

			const PIECE& color { b.take_turn() };
			bool succ;
			// succ = GreedyRollout(b, color);
			succ = RandomRollout(b, color);

			if ( !succ ) {
				if (color == BLACK)
					return WHITE_WIN;
				else
					return BLACK_WIN;
			}

			
		}
	}
	
	void backpropogate(const WIN_STATE &result, std::vector<TreeNode*> &path) {
		for (auto &node : path) {
			node->addresult(result);
		}
	}
	
	
	void tree_policy() {
		// auto start = chrono::steady_clock::now();
		board b {root_board};
		TreeNode *current;
		std::vector<TreeNode*> path;

		select(b, path);
		
		TreeNode &leaf_node = *(path.back());
		
		/** if "the leaf node have no child and have visit before"  **/
		if (leaf_node.child_size==0 && leaf_node.total_count.load() > 0){

			leaf_node.expand(b);

			if (leaf_node.child_size != 0) {
				current = UCB(&leaf_node);
				path.push_back(current);
				b.move(current->move.prev, current->move.next, current->color);
			}
			// no step can go
			else {
				const WIN_STATE result = ( (leaf_node.color==WHITE) ? WHITE_WIN : BLACK_WIN);
				backpropogate(result, path);
				return;
			}
		}

		const WIN_STATE result { simulate(b) };
		
		backpropogate(result, path);
	}


	void parallelLeaf_tree_policy() {
		// may have problem
		// int CountInSimulation = 0;
		board b {root_board};
		TreeNode *current;
		std::vector<TreeNode*> path;
		
		select(b, path);
		
		TreeNode &leaf_node = *(path.back());

		/** if "the leaf node have no child and have visit before"  **/
		if (leaf_node.child_size==0 && leaf_node.total_count.load() > 0){

			leaf_node.expand(b);

			if (leaf_node.child_size != 0) {
				current = UCB(&leaf_node);
				path.push_back(current);
				b.move(current->move.prev, current->move.next, current->color);
			}
			// no step can go
			else {
				const WIN_STATE result = ( (leaf_node.color==WHITE) ? WHITE_WIN : BLACK_WIN);
				backpropogate(result, path);
				return;
			}
		}
		omp_set_num_threads(THREAD_NUM);
		#pragma omp parallel for
		for (int i=0; i<THREAD_NUM; ++i){

			auto result = simulate(b);

			#pragma omp critical
			backpropogate(result, path);
		}
		
	}


	void parallelTree_tree_policy() {
		// auto start = chrono::steady_clock::now();
		board b {root_board};
		TreeNode *current;
		std::vector<TreeNode*> path;
		
		select(b, path);
		
		TreeNode &leaf_node = *(path.back());
		
		/** if "the leaf node have no child and have visit before"  **/
		if (leaf_node.child_size==0 && leaf_node.total_count.load() > 0){

			leaf_node.expandLock(b);

			if (leaf_node.child_size != 0) {
				current = UCB(&leaf_node);
				path.push_back(current);
				b.move(current->move.prev, current->move.next, current->color);
			}
			// no step can go
			else {
				const WIN_STATE result = ( (leaf_node.color==WHITE) ? WHITE_WIN : BLACK_WIN);
				backpropogate(result, path);
				return;
			}
		}

		const WIN_STATE result { simulate(b) };
		
		backpropogate(result, path);
	}
	

	void reset(board &b) {
		root_board = b;
		root = { std::make_unique<TreeNode>() };
		root->color = root_board.take_turn();
		root->move = {-1, -1};
		root->total_count.store(1);
		root->win_count.store(0);
		root->child = nullptr;
		root->child_size = 0;
		root->expand(b);
	}

};