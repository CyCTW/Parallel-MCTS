#include<iostream>
#include<memory>
#include<time.h>
#include<chrono>
#include<getopt.h>

#include "Agent.h"
#include "MCTS.h"
#include "Policy.h"
#include "Log.h"

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

    PIECE p = BLACK;
    
    board cur_board;
    Agent player{BLACK};
    Agent envir{WHITE};
    Log playerLog;
    Log envirLog;

    // Game Start
    while (true) {

        Pair mv;
        PIECE p = cur_board.take_turn();

        if ( p == BLACK ) {
            mv = player.take_action(cur_board, Policy::MCTS_Serial, playerLog);
        }
        else {
            mv = envir.take_action(cur_board, Policy::MCTS_Serial, envirLog);
        }

        // Game over
        if (mv == Pair{}) {
            break;
        }
        cout << cur_board << '\n';
    }

    cout << "Player\n\n";
    playerLog.printLog();
    
    cout << "Envir\n\n";
    envirLog.printLog();

   

}