#pragma once

#include <iostream>
#include <omp.h>
#include <map>
#include <thread>
#include "MCTS.h"
#include "Log.h"

using namespace std;
class Policy {
public:
    static Pair MCTS_Serial(board &before, const PIECE &piece, const EnvParameter &env,  Log &log) {

        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";

        int count_sim = 0;
        auto start = chrono::steady_clock::now();
        auto end = chrono::steady_clock::now();
        auto diff_time = chrono::duration<double, milli>(end - start).count();

        if ( env.simulation_counts > 0 ) {
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
        // cout << "Serial count:" << tree.root->total_count << "\n";
        log.search_count = tree.root->total_count;

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
                tree.parallelLeaf_tree_policy();
                end = chrono::steady_clock::now();
                diff_time = chrono::duration<double, milli>(end - start).count();
            }
        }
        else if(env.simulation_counts > 0) {
            while (count_sim < env.simulation_counts) {
                tree.parallelLeaf_tree_policy();
                count_sim++;
            }
        }
        // cout << "Parallel count:" << tree.root->total_count << "\n";
        log.search_count = tree.root->total_count;

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** leaf parallelization *****/

    /***** Root parallelization *****/

    // static void rootParallization(MonteCarloTree *tree) {

    //     // int count_sim = 0;
    //     // for(count_sim = 0; count_sim < SIMULATION_COUNT; count_sim++) {
    //     //     tree->tree_policy();
    //     // }
    //     auto start = chrono::steady_clock::now();
    //     auto end = chrono::steady_clock::now();
    //     auto diff_time = chrono::duration<double, milli>(end - start).count();
    //     while (diff_time < SIMULATION_TIME - COLLECT_BAG_TIME)
    //     {
    //         tree->tree_policy();
    //         end = chrono::steady_clock::now();
    //         diff_time = chrono::duration<double, milli>(end - start).count();
    //     }
    //     // cout << "Count sim: " << count_sim << '\n';
    // }
    // static void rootParallelizationPthread(MonteCarloTree *trees) {

    //     std::thread workers[ THREAD_NUM ];
    //     for(int i=1; i < THREAD_NUM; i++) {
    //         workers[i] = std::thread(Policy::rootParallization, &trees[i]);
    //     }
        
    //     Policy::rootParallization(&trees[0]);
    //     for(int i=1; i < THREAD_NUM; i++) {
    //         workers[i].join();
    //     }

    // }
    static void rootParallelizationOMP(MonteCarloTree *trees, const EnvParameter &env) {

        omp_set_num_threads(env.thread_num);

        if (env.time > 0) {
            #pragma omp parallel for
            for(int i = 0; i < env.thread_num; i++){
                auto start = chrono::steady_clock::now();
                auto end = chrono::steady_clock::now();
                auto diff_time = chrono::duration<double, milli>(end - start).count();

                while (diff_time < env.time)
                {
                    trees[i].tree_policy();
                    end = chrono::steady_clock::now();
                    diff_time = chrono::duration<double, milli>(end - start).count();
                }
                // cout << "Count: " << count_sim << '\n';
            }
        }
        else if (env.simulation_counts > 0) {
            #pragma omp parallel for
            for(int i = 0; i < env.thread_num; i++){
                int count_sim = 0;

                while (count_sim < env.simulation_counts) {
                    trees[i].tree_policy();
                    count_sim++;
                }
                // cout << "Count: " << count_sim << '\n';
            }
        }

    }
    static Pair MCTS_Parallel_Root(board &before, const PIECE &piece,  const EnvParameter &env,  Log &log) {

        MonteCarloTree trees[env.thread_num];
        for( int i = 0; i < env.thread_num; i++ ) {
            trees[i].reset(before);
        }

        Pair best_move;

        // rootParallelizationPthread(trees);
        rootParallelizationOMP(trees, env);
        auto start = chrono::steady_clock::now();

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
        // std::cout << "Parallel count: " << Vcount << "\n";
        log.search_count = Vcount;

        int maxCount = 0;
        auto end = chrono::steady_clock::now();
        auto diff_time = end - start;
        // std::cout << "collect bag time : " << chrono::duration<double, milli>(diff_time).count() << "\n";
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
    // static void treeParallization(MonteCarloTree *tree) {
    //     // int count_sim = 0;
    //     // for(count_sim = 0; count_sim < SIMULATION_COUNT; count_sim++) {
    //     //     tree->parallelTree_tree_policy();
    //     // }
    //     // cout << "Count sim: " << count_sim << '\n';
    //     auto start = chrono::steady_clock::now();
    //     auto end = chrono::steady_clock::now();
    //     auto diff_time = chrono::duration<double, milli>(end - start).count();
    //     while (diff_time < SIMULATION_TIME)
    //     {
    //         tree->parallelTree_tree_policy();
    //         end = chrono::steady_clock::now();
    //         diff_time = chrono::duration<double, milli>(end - start).count();
    //     }
    // }
    // static void treeParallizationPthread(MonteCarloTree *tree) {
        
    //     std::thread workers[THREAD_NUM];
    //     for(int i=1; i<THREAD_NUM; i++) {
    //         workers[i] = std::thread(Policy::treeParallization, tree);
    //     }
        
    //     Policy::treeParallization(tree);
    //     for(int i=1; i<THREAD_NUM; i++) {
    //         workers[i].join();
    //     }

    // }
    static void treeParallizationOMP(MonteCarloTree *tree,  const EnvParameter &env) {
        omp_set_num_threads(env.thread_num);

        if (env.simulation_counts > 0) {
            #pragma omp parallel for
            for (int i = 0; i < env.thread_num; ++i ){
                int count_sim = 0;
                while (count_sim < env.simulation_counts)
                {
                    tree->parallelTree_tree_policy();
                    count_sim++;
                }
            }
        }
        else if (env.time > 0) {
            #pragma omp parallel for
            for (int i = 0; i < env.thread_num; ++i ){
                auto start = chrono::steady_clock::now();
                auto end = chrono::steady_clock::now();
                auto diff_time = chrono::duration<double, milli>(end - start).count();
                
                while (diff_time < env.time)
                {
                    tree->parallelTree_tree_policy();
                    end = chrono::steady_clock::now();
                    diff_time = chrono::duration<double, milli>(end - start).count();
                }
            }
        }
    }
    static Pair MCTS_Parallel_Tree(board &before, const PIECE &piece,  const EnvParameter &env,  Log &log) { 
        MonteCarloTree tree;
        tree.reset(before);

        // std::cout << "MCTS take action\n";

        // cout << "Simulation time: " << env.simulation_counts << '\n';

        /***** Pthread *****/
        // treeParallizationPthread(&tree);
        /***** Pthread *****/
        
        /***** OpenMP *****/
        treeParallizationOMP(&tree, env);
        /***** OpenMP *****/
        

        // cout << "Parallel count:" << tree.root->total_count << "\n";
        log.search_count = tree.root->total_count;

        if (PRINT_TREE)
            tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }
    /***** Tree parallelization *****/

};