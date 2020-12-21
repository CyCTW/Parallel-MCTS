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
    cout << "   -?  --help                      Edit config.h to set parameter\n";
}
void setEnvParameter(int &simulation_counts, int argc, char** argv) {
    int opt;
    static struct option long_options[] = {
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "v:c:?", long_options, NULL)) != EOF)
    {
        switch (opt)
        {
        case 'c':
            simulation_counts = atoi(optarg);
            break;
        case '?':
        default:
            usage();
            exit(0);
        }
    }
}

auto setPolicy(string policy) {
    switch (policy[0]) {
        case 'S':
        case 's':
            return Policy::MCTS_Serial;
        case 'L':
        case 'l':
            return Policy::MCTS_Parallel_Leaf;
            break;
        case 'R':
        case 'r':
            return Policy::MCTS_Parallel_Root;
            break;
        case 'T':
        case 't':
            return Policy::MCTS_Parallel_Tree;
            break;
        default:
            std::cerr << "Black Policy not defined!\n";
            exit(0);
    }
}
int main(int argc, char *argv[]) {

    /***** Parameter Setting *****/
    int simulation_counts = SIMULATION_COUNT;
    const int limitStep = LIMIT_STEP;
    auto black_policy = setPolicy(BLACK_POLICY);
    auto white_policy = setPolicy(WHITE_POLICY);
    
    setEnvParameter(simulation_counts, argc, argv);
    
    Log playerLog;
    Log envirLog;

    // Game Parameter
    board cur_board;
    Agent player{BLACK};
    Agent envir{WHITE};
    WIN_STATE outcome;

    while (true) {

        Pair mv;
        PIECE p = cur_board.take_turn();

        if ( p == BLACK ) {
            mv = player.take_action(cur_board, black_policy, playerLog);
        }
        else {
            mv = envir.take_action(cur_board, white_policy, envirLog);
        }

        // Game over
        if (mv == Pair{}) {
            outcome = cur_board.compare_piece();
            break;
        } else if (player.getStep() > limitStep ) {
            outcome = cur_board.compare_piece();
            break;
        }
        cout << cur_board << '\n';
    }
    cout << "winner:  ";
    if (outcome == BLACK_WIN) {
        cout << "Black\n";

    } else if (outcome == WHITE_WIN) {
        cout << "White\n";
    } else {
        cout << "Draw\n";
    }
    
    cout << "Player\n\n";
    playerLog.printLog();
    
    cout << "Envir\n\n";
    envirLog.printLog();

   

}