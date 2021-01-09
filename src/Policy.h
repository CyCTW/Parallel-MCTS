#pragma once

#include "MCTS.h"
#include "Log.h"
#include "ParallelRoot.h"
#include "ParallelTree.h"

#include <iostream>
#include <omp.h>
#include <map>

using namespace std;

class Policy {
public:
    static Pair MCTS_Serial(board &before, const PIECE &piece, const EnvParameter &env,  Log &log) {

        MonteCarloTree tree;
        tree.reset(before);
        auto start = chrono::steady_clock::now();
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration<double, milli>(end - start).count();

        if ( env.simulation_counts > 0 ) {
            int count_sim = 0;

            while (count_sim < env.simulation_counts) {
                tree.tree_policy();
                count_sim++;
            }

        }
        else if ( env.time > 0 ){
            
            while(diff_time < env.time ){
                tree.tree_policy();
                end = chrono::steady_clock::now();
                diff_time = chrono::duration<double, milli>(end - start).count();
            }
        }
        log.search_count += tree.root->total_count;
        log.cost_time += diff_time;

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        
        //std::cerr << "choose: (" << best_move.prev/6 << ", "<< best_move.prev%6  << ") to (" << best_move.next/6 << ", " <<best_move.next%6 << ")  \n";
        return best_move;
    }

    /***** leaf parallelization *****/
    static Pair MCTS_Parallel_Leaf(board &before, const PIECE &piece, const EnvParameter &env,  Log &log) { 
        MonteCarloTree tree;
        tree.reset(before);
        // std::cout << "MCTS take action\n";

        int count_sim = 0;
        auto start = chrono::steady_clock::now();
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration<double, milli>(end - start).count();

        if (env.time > 0) {
            while (diff_time < env.time)
            {
                tree.parallelLeaf_tree_policy(env);
                end = chrono::steady_clock::now();
                diff_time = chrono::duration<double, milli>(end - start).count();
            }
        }
        else if(env.simulation_counts > 0) {
            while (count_sim < env.simulation_counts) {
                tree.parallelLeaf_tree_policy(env);
                count_sim++;
            }
        }
        // cout << "Parallel count:" << tree.root->total_count << "\n";
        log.search_count = tree.root->total_count;
        log.cost_time += diff_time;

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** leaf parallelization *****/

    /***** Root parallelization *****/
    static Pair MCTS_Parallel_Root(board &before, const PIECE &piece,  const EnvParameter &env,  Log &log) {

        MonteCarloTree trees[env.thread_num];
        for( int i = 0; i < env.thread_num; i++ ) {
            trees[i].reset(before);
        }

        Pair best_move;
        auto start = chrono::steady_clock::now();
        if (piece == BLACK) {
            if(env.black_method[0] == 'p')
                ParallelRoot::Pthread(trees, env);
            else if (env.black_method[0] == 'o')
                ParallelRoot::OMP(trees, env);
        } else {
            if(env.white_method[0] == 'p')
                ParallelRoot::Pthread(trees, env);
            else if (env.white_method[0] == 'o')
                ParallelRoot::OMP(trees, env);
        }
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration<double, milli>(end-start).count();

        map<Pair, double> bag;
        bag.clear();
        int Vcount = 0;

        // aggregate count result
        for(int thread_idx = 0; thread_idx < env.thread_num; thread_idx++){
            // get child & count
            auto child = trees[ thread_idx ].root->get_child();
            Vcount += trees[ thread_idx ].root->total_count;
            size_t child_size = trees[ thread_idx ].root->get_child_size();
            
            if (child_size == 0) {
                return {};
            }

            for(size_t child_idx = 0; child_idx < child_size; child_idx++){
                Pair move = child[ child_idx ].get_move();
                bag[move] += child[ child_idx ].total_count;
            }
        }
        log.search_count = Vcount;
        log.cost_time += diff_time;

        int maxCount = 0;
        for(auto mp : bag) {
            if (PRINT_TREE)
                std::cout << (int)mp.first.prev << ", " << (int)mp.first.next << ":  " << mp.second << '\n';
            if (mp.second > maxCount) {
                maxCount = mp.second;
                best_move = mp.first;
            }
        }

        return best_move;
    }
    /***** Root parallelization *****/


    /***** Tree parallelization *****/    
    static Pair MCTS_Parallel_Tree(board &before, const PIECE &piece,  const EnvParameter &env,  Log &log) { 
        MonteCarloTree tree;
        tree.reset(before);

        auto start = chrono::steady_clock::now();
        if (piece == BLACK) {
            if (env.black_method[0] == 'p') {
                ParallelTree::Pthread(&tree, env);
            } else if (env.black_method[0] == 'o') {
                ParallelTree::OMP(&tree, env);
            }
        } else {
            if (env.white_method[0] == 'p') {
                ParallelTree::Pthread(&tree, env);
            } else if (env.white_method[0] == 'o') {
                ParallelTree::OMP(&tree, env);
            }
        }
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration<double, milli>(end-start).count();
        // cout << "Parallel count:" << tree.root->total_count << "\n";
        log.search_count = tree.root->total_count;
        log.cost_time += diff_time;

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** Tree parallelization *****/

};