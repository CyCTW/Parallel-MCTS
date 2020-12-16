#pragma once

#include "TreeNode.h"

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <climits>
#include <omp.h>
#define THREAD_NUM 4

class MonteCarloTree {
public:
	std::unique_ptr<TreeNode> root;
	std::vector<TreeNode*> path;
	board root_board;
	
	std::random_device rd;
	std::default_random_engine eng;

	static constexpr double explore_parameter = sqrt(2.0);

	MonteCarloTree() : root(), path(), root_board(), eng(rd()) {}
	// MonteCarloTree() : root(), path(), root_board(), eng(time(NULL)) {}
	TreeNode* UCB (TreeNode* n)  {

		if(n->child_size == 0) return nullptr;
		
		constexpr double eps = 1e-3;
		double max_score = INT_MIN;
		std::size_t same_score[100]{};
		std::size_t idx = 0;

		for (std::size_t i = 0; i < n->child_size; ++i) {
			TreeNode* ch = n->child.get() + i;
			
			const double exploit { ch->win_count / (double)(ch->total_count + 1.0) };
			const double explore { sqrt( log(n->total_count) / (double)(ch->total_count+1.0) ) };
			const double score { exploit + explore_parameter * explore };
			
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
		
		return (n->child.get() + best_idx); 
	}

	void select(board &b) {

		TreeNode* current { root.get() };
	
		path.clear();
		path.push_back(current);

		while (current->child != nullptr && current->child_size != 0) {
			current = UCB(current);
			path.push_back(current);

			b.move(current->move.prev, current->move.next, current->color);
		}
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
			std::vector<Pair> ea { b.find_piece(color, EAT) };
			std::vector<Pair> mv { b.find_piece(color, MOVE) };
			
			if (!ea.empty()) {
				std::shuffle(ea.begin(), ea.end(), eng);
				b.move(ea[0].prev, ea[0].next, color);
			}
			else if (!mv.empty()){
				std::shuffle(mv.begin(), mv.end(), eng);
				b.move(mv[0].prev, mv[0].next, color);
			}
			else{
				if (color == BLACK)
					return WHITE_WIN;
				else
					return BLACK_WIN;
			}
		}
	}
	
	void backpropogate(const WIN_STATE &result) {
		for (auto &node : path) {
			node->addresult(result);
		}
	}
	
	void tree_policy() {
		// auto start = chrono::steady_clock::now();
		board b {root_board};
		TreeNode *current;
		
		select(b);
		
		TreeNode &leaf_node = *(path.back());
		
		/** if "the leaf node have no child and have visit before"  **/
		if (leaf_node.child_size==0 && leaf_node.total_count > 0){

			leaf_node.expand(b);

			if (leaf_node.child_size != 0) {
				current = UCB(&leaf_node);
				path.push_back(current);
				b.move(current->move.prev, current->move.next, current->color);
			}
			// no step can go
			else {
				const WIN_STATE result = ( (leaf_node.color==WHITE) ? WHITE_WIN : BLACK_WIN);
				backpropogate(result);
				return;
			}
		}

		const WIN_STATE result { simulate(b) };
		
		backpropogate(result);
		// auto end = chrono::steady_clock::now();
  //       auto diff_time = end - start;
  //       printf("Serial policy takes %f ms\n", chrono::duration <double, milli> (diff_time).count());
	}
	void parallel_tree_policy() {
		// may have problem
		// auto start = chrono::steady_clock::now();
		int CountInSimulation = 0;
		board b {root_board};
		TreeNode *current;
		
		select(b);
		
		TreeNode &leaf_node = *(path.back());

		/** if "the leaf node have no child and have visit before"  **/
		if (leaf_node.child_size==0 && leaf_node.total_count > 0){

			leaf_node.expand(b);

			if (leaf_node.child_size != 0) {
				current = UCB(&leaf_node);
				path.push_back(current);
				b.move(current->move.prev, current->move.next, current->color);
			}
			// no step can go
			else {
				const WIN_STATE result = ( (leaf_node.color==WHITE) ? WHITE_WIN : BLACK_WIN);
				backpropogate(result);
				return;
			}
		}
		omp_set_num_threads(THREAD_NUM);

		// WIN_STATE results[8];
		#pragma omp parallel for
		for (int i=0; i<THREAD_NUM; ++i){

			// results[i] = simulate(b) ;
			auto result = simulate(b);

			#pragma omp critical
			backpropogate(result);
			
		}
		// int black = 0, white = 0;
		// for(int i=0; i<thread_num; ++i) {
		// 	if (results[i] == BLACK_WIN) black++;
		// 	else white++;
		// }
		// if (black > white) backpropogate(BLACK_WIN);
		// else backpropogate(WHITE_WIN);

		// auto end = chrono::steady_clock::now();
  //       auto diff_time = end - start;
  //       printf("Parallel policy takes %f ms\n", chrono::duration <double, milli> (diff_time).count());
		
	}
	

	void reset(board &b) {
		root_board = b;
		root = { std::make_unique<TreeNode>() };
		root->color = root_board.take_turn();
		root->move = {-1, -1};
		root->total_count = 1;
		root->win_count = 0;
		root->child = nullptr;
		root->child_size = 0;
		root->expand(b);
	}

};