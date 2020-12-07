#pragma once

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
        
        // tree.root->showchild();

        Pair best_move = tree.root->best_child();
        return best_move;
    }

    static Pair MCTS_Parallel(board &before, const PIECE &piece, const int &simulation_times) { 
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

};