#pragma once

#include <iostream>
#include <omp.h>
#include <map>
#include <thread>
#include "MCTS.h"

class Policy {
public:
    static Pair MCTS_Serial(board &before, const PIECE &piece) {
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";

        int count_sim = 0;
        while (count_sim < SIMULATION_COUNT) {
            tree.tree_policy();
            count_sim++;
        }
        cout << "Serial count:" << tree.root->total_count << "\n";
        
        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        //std::cerr << "choose: (" << best_move.prev/6 << ", "<< best_move.prev%6  << ") to (" << best_move.next/6 << ", " <<best_move.next%6 << ")  \n";
        return best_move;
    }

    /***** leaf parallelization *****/
    static Pair MCTS_Parallel_Leaf(board &before, const PIECE &piece) { 
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";

        int count_sim = 0;
        while (count_sim < SIMULATION_COUNT) {
            tree.parallelLeaf_tree_policy();
            count_sim++;
        }
        cout << "Parallel count:" << tree.root->total_count << "\n";

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** leaf parallelization *****/

    /***** Root parallelization *****/

    static void rootParallization(MonteCarloTree *tree) {
        int count_sim = 0;
        for(count_sim = 0; count_sim < SIMULATION_COUNT; count_sim++) {
            tree->tree_policy();
        }
        // cout << "Count sim: " << count_sim << '\n';
    }
    static void rootParallelizationPthread(MonteCarloTree *trees) {
        std::thread workers[ THREAD_NUM ];
        for(int i=1; i < THREAD_NUM; i++) {
            workers[i] = std::thread(Policy::rootParallization, &trees[i]);
        }
        
        Policy::rootParallization(&trees[0]);
        for(int i=1; i < THREAD_NUM; i++) {
            workers[i].join();
        }
    }
    static void rootParallelizationOMP(MonteCarloTree *trees) {

        omp_set_num_threads(THREAD_NUM);

        #pragma omp parallel for
        for(int i = 0; i < THREAD_NUM; i++){
            int count_sim = 0;
            while (count_sim < SIMULATION_COUNT) {
                trees[i].tree_policy();
                count_sim++;
            }
            // cout << "Count: " << count_sim << '\n';
        }
    }
    static Pair MCTS_Parallel_Root(board &before, const PIECE &piece) { 
        
        MonteCarloTree trees[THREAD_NUM];
        for(int i=0; i < THREAD_NUM; i++) {
            trees[i].reset(before);
        }

        Pair best_move;
        rootParallelizationPthread(trees);
        // rootParallelizationOMP(trees);

        map<Pair, double> bag;
        bag.clear();
        int Vcount = 0;

        // aggregate count result
        for(int thread_idx = 0; thread_idx < THREAD_NUM; thread_idx++){
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
        std::cout << "Parallel count: " << Vcount << "\n";

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
    static void treeParallization(MonteCarloTree *tree) {
        int count_sim = 0;
        for(count_sim = 0; count_sim < SIMULATION_COUNT; count_sim++) {
            tree->parallelTree_tree_policy();
        }
        cout << "Count sim: " << count_sim << '\n';
    }
    static void treeParallizationPthread(MonteCarloTree *tree) {
        
        std::thread workers[THREAD_NUM];
        for(int i=1; i<THREAD_NUM; i++) {
            workers[i] = std::thread(Policy::treeParallization, tree);
        }
        
        Policy::treeParallization(tree);
        for(int i=1; i<THREAD_NUM; i++) {
            workers[i].join();
        }

    }
    static void treeParallizationOMP(MonteCarloTree *tree) {
        omp_set_num_threads(THREAD_NUM);

        #pragma omp parallel for
        for(int count_sim = 0; count_sim < SIMULATION_COUNT; count_sim++) {
            tree->parallelTree_tree_policy();
            count_sim++;
        }
    }
    static Pair MCTS_Parallel_Tree(board &before, const PIECE &piece) { 
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";

        cout << "Simulation time: " << SIMULATION_COUNT << '\n';

        /***** Pthread *****/
        treeParallizationPthread(&tree);
        /***** Pthread *****/
        
        /***** OpenMP *****/
        // treeParallizationOMP(&tree);
        /***** OpenMP *****/
        

        cout << "Parallel count:" << tree.root->total_count << "\n";
        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** Tree parallelization *****/

};