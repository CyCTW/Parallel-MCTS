class ParallelLeaf {

    static void OMP(EnvParameter env) {
        omp_set_num_threads(env.thread_num);
        
        #pragma omp parallel for
        for (int i=0; i < env.thread_num; ++i){

            auto result = simulate(b);

            #pragma omp critical
            backpropogate(result, path);
        }
    }
}