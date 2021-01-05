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
    cout << "   -c  --simCount                     Simulation count per step\n";
    cout << "   -t  --simTime                      Simulation time per step\n";
    cout << "   -T  --threadNum                    Number of thread num\n";
    cout << "   -b  --blackPolicy                  Black policy (e.g. Serial, Leaf, Root, Tree)\n";
    cout << "   -w  --whitePolicy                  White policy (e.g. Serial, Leaf, Root, Tree)\n";
    cout << "   -?  --help                         Edit config.h to set parameter\n";
}
void setEnvParameter(int argc, char** argv) {
    int opt;
    static struct option long_options[] = {
        {"simCount", 0, 0, 'c'},
        {"simTime", 0, 0, 't'},
        {"threadNum", 0, 0, 'T'},
        {"blackPolicy", 0, 0, 'b'},
        {"whitePolicy", 0, 0, 'w'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};
    
    
    while ((opt = getopt_long(argc, argv, "c:t:T:b:w:?", long_options, NULL)) != EOF)
    {
        cout << char(opt) << '\n';
        switch (opt)
        {
            case 'c':
                envParam.simulation_counts = atoi(optarg);
                envParam.time = -1;
                break;
            case 't':
                envParam.time = atoi(optarg);
                envParam.simulation_counts = -1;
                break;
            case 'T':
                envParam.thread_num = atoi(optarg);
                break;
            case 'b':
                envParam.black_policy = optarg;
                break;
            case 'w':
                envParam.white_policy = optarg;
                break;
            case '?':
                usage();
                exit(0);
                break;
            default:
                exit(0);
        }
    }
    if (envParam.time >= 0 && envParam.simulation_counts >= 0) {
        cout << "Can't set Count and Time simutaneously!\n";
        exit(0);
    }
}

auto setPolicy(string policy) {
    cout << policy << '\n';
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
    envParam = {-1, 1000, 4, "Tree", "Serial"};
    setEnvParameter(argc, argv);

    int simulation_counts = envParam.simulation_counts;
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