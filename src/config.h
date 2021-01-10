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
    cout << "   -c  --simCount   <simulationCount>           Set simulation count per step.       default: 1000\n";
    cout << "   -t  --simTime    <simulationTime (second)>   Set simulation time per step.        default: -1\n";
    cout << "   -T  --threadNum  <threadNum>                 Set number of thread num.            default: 4\n";
    cout << "   -p  --policy     <blackAndWhitePolicy>       Set Player's policy \n"\
            "                    <blackPolicy> <whitePolicy>  (options: Serial, Leaf, Root, Tree) default: Tree Serial)\n\n";

    cout << "   -m  --method     <blackAndWhiteMethod>       Set Player's arallel methods \n"\
            "                    <blackMethod> <whiteMethod>  (options: openmp, pthread)          default: openmp)\n\n";
    cout << "   -?  --help                                   Help message.\n";
}
void setEnvParameter(int argc, char** argv) {
    int opt;
    static struct option long_options[] = {
        {"simCount", 0, 0, 'c'}, {"simTime", 0, 0, 't'}, {"threadNum", 0, 0, 'T'},  {"policy", 0, 0, 'p'},  {"method", 0, 0, 'm'}, {"help", 0, 0, '?'},  {0, 0, 0, 0}};

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
                usage(); exit(0);
                break;
            default:
                usage(); exit(0);
        }
    }
}

