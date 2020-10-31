#include<iostream>
#include<memory>
#include<time.h>
#include<chrono>
#include<getopt.h>
#include "MCTS.h"

using namespace std;

Pair MCTS_Serial(board &before, const PIECE &piece, const int &simulation_times=1000) {
    MonteCarloTree tree;
    tree.reset(before);

    std::cout << "MCTS take action\n";
    const int &simulationtime = simulation_times;

    int count_sim = 0;
    while (count_sim < simulationtime) {
        tree.tree_policy();
        count_sim++;
    }
    
    // tree.root->showchild();

    Pair best_move = tree.root->best_child();
    return best_move;
}

void MCTS_Parallel() {
    return;
}

void usage() {
    cout << "Usage: mcts [options]\n";
    cout << "   -v  --version <serial/thread>   Enter which version to run\n";
    cout << "   -c  --count <N>                 Run MCTS with N simulation times\n";
    cout << "   -?  --help                      Help message\n";
}

int main(int argc, char *argv[]) {
    int simulation_counts = 1000;
    string version = "serial";
    // parse argument
    int opt;
    static struct option long_options[] = {
        {"version", 1, 0, 'v'},
        {"count", 1, 0, 'c'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "v:c:?", long_options, NULL)) != EOF)
    {
        switch (opt)
        {
        case 'v':
            version = string(optarg);
            break;
        case 'c':
            simulation_counts = atoi(optarg);
            break;
        case '?':
        default:
            usage();
            return 1;
        }
    }

    board init = board();
    PIECE p = BLACK;

    auto start = chrono::steady_clock::now();
    if (version == "serial") {
        MCTS_Serial( init, p, simulation_counts);    
    }
    else{
        MCTS_Parallel();
    }
    auto end = chrono::steady_clock::now();
    auto diff = end - start;

    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}