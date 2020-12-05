#include<iostream>
#include<memory>
#include<time.h>
#include<chrono>
#include<getopt.h>

#include "Agent.h"
#include "MCTS.h"
#include "Policy.h"

using namespace std;



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
    
    board cur_board;
    Agent player{BLACK};
    Agent envir{WHITE};


    // Game Start
    while (true) {

        Pair mv;
        PIECE p = cur_board.take_turn();

        if (p == BLACK ) {
            mv = player.take_action(cur_board, Policy::MCTS_Serial);
        }
        else {
            mv = envir.take_action(cur_board, Policy::MCTS_Serial);
        }

        // Game over
        if (mv == Pair{}) {
            break;
        }
        cout << cur_board << '\n';
    }

    auto end = chrono::steady_clock::now();
    auto diff = end - start;

    cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}