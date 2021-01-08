#include<thread>


class ParallelTree{
public:
    static void treeParallization(MonteCarloTree *tree, EnvParameter env) {
        
        if (env.time > 0) {
            auto start = chrono::steady_clock::now();
            auto end = chrono::steady_clock::now();
            auto diff_time = chrono::duration<double, milli>(end - start).count();

            while (diff_time < env.time)
            {
                tree->parallelTree_tree_policy();
                end = chrono::steady_clock::now();
                diff_time = chrono::duration<double, milli>(end - start).count();
            }
        } else if (env.simulation_counts > 0) {
            int count_sim = 0;

            while(count_sim < env.simulation_counts) {
                tree->parallelTree_tree_policy();
                count_sim++;
            }
        }
    }
    static void Pthread(MonteCarloTree *tree, EnvParameter env) {
        
        std::thread workers[ env.thread_num ];
        for(int i=1; i < env.thread_num; i++) {
            workers[i] = std::thread(ParallelTree::treeParallization, tree, env);
        }
        
        ParallelTree::treeParallization(tree, env);
        for(int i=1; i < env.thread_num; i++) {
            workers[i].join();
        }
    }
    /***** Pthread *****/

    /***** OpenMP *****/
    static void OMP(MonteCarloTree *tree,  const EnvParameter &env) {
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
    /***** OpenMP *****/
};