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
    cout << "\nNote: -c (count) & -t (time) options can't be specified simultaneously.\n\n";
    cout << "   -c  --simCount                     Simulation count per step\n";
    cout << "   -t  --simTime                      Simulation time per step\n";
    cout << "   -T  --threadNum                    Number of thread num\n";
    cout << "   -p  --policy                       Black & White policy (choice: Serial, Leaf, Root, Tree, defulat: Tree)\n"\
            "                        <Policy> set both Black and White Policy\n"\
            "                        <Policy> <Policy> set Black and White Policy respectively\n";

    cout << "   -m  --method                       methods ( choice: openmp, pthread, default: openmp )\n"\
            "                        <Method> set both Black and White Method\n"\
            "                        <Method> <Method> set Black and White Method respectively\n";
    cout << "   -?  --help                         Edit config.h to set parameter\n";
}
void setEnvParameter(int argc, char** argv) {
    int opt;
    static struct option long_options[] = {
        {"simCount", 0, 0, 'c'},
        {"simTime", 0, 0, 't'},
        {"threadNum", 0, 0, 'T'},
        {"policy", 0, 0, 'p'},
        {"method", 0, 0, 'm'},
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, "c:t:T:p:m:?", long_options, NULL)) != EOF)
    {
        // std::string argPolicy = optarg;
        cout << "argC: " << argc << '\n';
        // cout << "argV: " << string(argv) << '\n';
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
            case 'p':
                envParam.black_policy = optarg;
                envParam.white_policy = optarg;
                break;
            case 'm':
                envParam.black_method =  optarg;
                envParam.white_method =  optarg;

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
    envParam = {1000, -1, 4, "Tree", "Serial", "openmp", "openmp"};
    setEnvParameter(argc, argv);
    cout << "simulationCount: " << envParam.simulation_counts << '\n';
    cout << "time: " << envParam.time << '\n';
    cout << "thread num: " << envParam.thread_num << '\n';
    cout << "Black policy: " << envParam.black_policy << '\n';
    cout << "White policy: " << envParam.white_policy << '\n';
    cout << "Black method: " << envParam.black_method << '\n';
    cout << "White method: " << envParam.white_method << '\n';
    cout << "\n\n";
    const int limitStep = LIMIT_STEP;
    auto black_policy = setPolicy( envParam.black_policy );
    auto white_policy = setPolicy( envParam.white_policy );
    
    return 0;
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