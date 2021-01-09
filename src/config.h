#define LIMIT_STEP 500          // step limits in a game
#define PRINT_TREE 0            // Print tree for debugging
#include <getopt.h>

using namespace std;

struct EnvParameter{
    int simulation_counts;
    double time;
    int thread_num;
    std::string black_policy;
    std::string white_policy;
	std::string black_method;
	std::string white_method;


} envParam;

void usage() {
    cout << "Usage: mcts [options]\n";
    cout << "\nNote: -c (count) & -t (time) options can't be specified simultaneously.\n\n";
    cout << "   -c  --simCount   <simulationCount>           Set simulation count per step\n";
    cout << "   -t  --simTime    <simulationTime>            Set simulation time per step\n";
    cout << "   -T  --threadNum  <threadNum>                 Set number of thread num\n";
    cout << "   -p  --policy     <blackAndWhitePolicy>       Set Player's policy \n"\
            "                    <blackPolicy> <whitePolicy> ( choice: Serial, Leaf, Root, Tree. default: Tree Serial)\n";

    cout << "   -m  --method     <blackAndWhiteMethod>       Set Player's arallel methods \n"\
            "                    <blackMethod> <whiteMethod> ( choice: openmp, pthread.  default: openmp )\n";
    cout << "   -?  --help                                   Edit config.h to set parameter\n";
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
        int index;
        switch (opt)
        {
            case 'c':
                envParam.simulation_counts = atoi(optarg);
                envParam.time = -1;
                break;
            case 't':
                envParam.time = atoi(optarg) * 1000; // convert s to ms
                envParam.simulation_counts = -1;
                break;
            case 'T':
                envParam.thread_num = atoi(optarg);
                break;
            case 'p':
                index = optind - 1;
                if (index + 1 < argc) {
                    if ( argv[index+1][0] == '-') {
                        envParam.black_policy = envParam.white_policy = argv[index];
                    } else {
                        envParam.black_policy =  argv[index];
                        envParam.white_policy =  argv[index+1];
                    }
                } else {
                        envParam.black_policy = envParam.white_policy = argv[index];
                }
                break;
            case 'm':
                index = optind - 1;
                if (index + 1 < argc) {
                    if ( argv[index+1][0] == '-') {
                        envParam.black_method = envParam.white_method = argv[index];
                    } else {
                        envParam.black_method =  argv[index];
                        envParam.white_method =  argv[index+1];
                    }
                } else {
                    envParam.black_method = envParam.white_method = argv[index];
                }
                break;
            case '?':
                usage();
                exit(0);
                break;
            default:
                cout << "Argument not recognized. Please see the usage below:\n\n";
                exit(0);
        }
    }
    if (envParam.time >= 0 && envParam.simulation_counts >= 0) {
        std::cout << "Can't set Count and Time simutaneously!\n";
        exit(0);
    }
}

