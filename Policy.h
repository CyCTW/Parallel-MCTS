#pragma once
struct Move_and_Count{
    Pair mv;
    double cnt;
};
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

    /*** root parallelization ***/
    static Pair MCTS_Parallel_root(board &before, const PIECE &piece, const int &simulation_times) { 
        MonteCarloTree tree[4];
        Pair best_move;
        vector<Move_and_Count> vec;
#pragma omp parallel for
        for(int i = 0;i < 4;i++){
            //std::cout << "start root parallelization: " << i << std::endl;
            tree[i].reset(before);
            const int &simulationtime = simulation_times;
            int count_sim = 0;
            while (count_sim < simulationtime) {
                tree[i].tree_policy();
                count_sim++;
            }
        }
        for(int i = 0;i < 4;i++){
            // get child & count
            auto child = tree[i].root->get_child();
            size_t child_size = tree[i].root->get_child_size();
            if (child_size == 0) {
                return {};
            }
            for(int j = 0;j < child_size;j++){
                bool do_exist = false;
                Pair move = child[i].get_move();
                for(int k = 0;k < vec.size();k++){
                    if(vec[k].mv == move)
                        do_exist = true;
                        vec[k].cnt += child[i].total_count;
                        break;
                }
                if(do_exist == false){
                    Move_and_Count tmp;
                    tmp.mv = move;
                    tmp.cnt = child[i].total_count;
                    vec.push_back(tmp);
                }
            }
        }
        double max=0;
        for(int i = 0;i < vec.size();i++){
            if(vec[i].cnt > max){
                max = vec[i].cnt;
                best_move = vec[i].mv;
            }
        }
        return best_move;
    }

};