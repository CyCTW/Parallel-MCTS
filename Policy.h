#pragma once

#include <iostream>
#include <omp.h>
#include <map>
#include "MCTS.h"

class Policy {
public:
    static Pair MCTS_Serial(board &before, const PIECE &piece, const int &simulation_times) {
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";
        const int &simulationtime = simulation_times;

        int count_sim = 0;
        while (count_sim < simulationtime) {
            tree.tree_policy();
            count_sim++;
        }
        cout << "Serial count:" << tree.root->total_count << "\n";
        
        //tree.root->showchild();

        Pair best_move = tree.root->best_child();
        //std::cerr << "choose: (" << best_move.prev/6 << ", "<< best_move.prev%6  << ") to (" << best_move.next/6 << ", " <<best_move.next%6 << ")  \n";
        return best_move;
    }

    /*** leaf parallelization ***/
    static Pair MCTS_Parallel_Leaf(board &before, const PIECE &piece, const int &simulation_times) { 
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";
        const int &simulationtime = simulation_times;

        int count_sim = 0;
        while (count_sim < simulationtime) {
            tree.parallel_tree_policy();
            count_sim++;
        }
        cout << "Parallel count:" << tree.root->total_count << "\n";
        // tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }

    /*** root parallelization ***/
    static Pair MCTS_Parallel_Root(board &before, const PIECE &piece, const int &simulation_times) { 
        
        omp_set_num_threads(THREAD_NUM);
        
        MonteCarloTree tree[THREAD_NUM];
        Pair best_move;
        
        #pragma omp parallel for
        for(int i = 0; i < THREAD_NUM; i++){
            tree[i].reset(before);
            const int &simulationtime = simulation_times;
            int count_sim = 0;
            while (count_sim < simulationtime) {
                tree[i].tree_policy();
                count_sim++;
            }
        }

        map<Pair, double> bag;
        bag.clear();
        int Vcount = 0;

        // aggregate count result
        for(int thread_idx = 0; thread_idx < THREAD_NUM; thread_idx++){
            // get child & count
            auto child = tree[ thread_idx ].root->get_child();
            Vcount += tree[ thread_idx ].root->total_count;
            size_t child_size = tree[ thread_idx ].root->get_child_size();
            
            if (child_size == 0) {
                return {};
            }

            for(size_t child_idx = 0; child_idx < child_size; child_idx++){
                Pair move = child[ child_idx ].get_move();
                bag[move] += child[ child_idx ].total_count;
            }
        }
        // std::cout << "Visit count: " << Vcount << "\n";

        int maxCount = 0;
        for(auto mp : bag) {
            if (mp.second > maxCount) {
                maxCount = mp.second;
                best_move = mp.first;
            }
        }
        return best_move;
    }
    
    /*** tree parallelization ***/
    static Pair MCTS_Parallel_Tree(board &before, const PIECE &piece, const int &simulation_times) { 
        MonteCarloTree tree;
        tree.reset(before);
        omp_set_num_threads(THREAD_NUM);

        // std::cout << "MCTS take action\n";
        const int &simulationtime = simulation_times * THREAD_NUM;

        #pragma omp parallel for
        for(int count_sim = 0; count_sim < simulationtime; count_sim++) {
            tree.tree_policy_parallel();
            count_sim++;
        }
        cout << "Parallel count:" << tree.root->total_count << "\n";
        // tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
};