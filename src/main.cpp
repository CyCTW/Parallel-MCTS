#include <iostream>
#include <memory>
#include <time.h>
#include <chrono>

#include "Agent.h"
#include "MCTS.h"
#include "Policy.h"
#include "Log.h"

using namespace std;

auto setPolicy(string policy) {
    // cout << policy << '\n';
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

    // default parameters
    envParam = {1000, -1, 4, "Tree", "Serial", "openmp", "openmp"};
    setEnvParameter(argc, argv);
    cout << "simulationCount: " << envParam.simulation_counts << '\n';
    cout << "time: " << envParam.time << " ms" << '\n';
    cout << "thread num: " << envParam.thread_num << '\n';
    cout << "Black policy: " << envParam.black_policy << '\n';
    cout << "White policy: " << envParam.white_policy << '\n';
    cout << "Black method: " << envParam.black_method << '\n';
    cout << "White method: " << envParam.white_method << '\n';
    cout << "\n\n";
    const int limitStep = LIMIT_STEP;
    auto black_policy = setPolicy( envParam.black_policy );
    auto white_policy = setPolicy( envParam.white_policy );
    
    /***** Parameter Setting *****/
    
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
            mv = player.take_action(cur_board, black_policy, playerLog, envParam);
        }
        else {
            mv = envir.take_action(cur_board, white_policy, envirLog, envParam);
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