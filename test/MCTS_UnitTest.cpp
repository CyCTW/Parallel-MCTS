#include "gtest/gtest.h"

#include "../src/Agent.h"
#include "../src/board.h"
#include "../src/Policy.h"
#include "../src/MCTS.h"
#include "../src/Log.h"
using namespace std;

TEST(SimulationCount, SerialSimulationCount) {
	const int simulation_count = 100;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, 0, "","", "openmp"};
	Log log;

	Policy::MCTS_Serial(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count);
}	
TEST(SimulationCount, ParallelLeafSimulationCount) {
	const int simulation_count = 100;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Leaf(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}

TEST(SimulationCount, ParallelRootSimulationCount) {
	const int simulation_count = 100;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Root(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);

}
TEST(SimulationCount, ParallelTreeSimulationCount) {
	const int simulation_count = 100;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Tree(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}

TEST(SimulationTime, SerialSimulationTime) {
	const double time = 0.5;
	
	board b;
	PIECE p = BLACK;
	EnvParameter env = {-1, time, 0, "","",  "openmp"};
	Log log;

	Policy::MCTS_Serial(b, p, env, log);
	// EXPECT_EQ(log.search_count, simulation_count);
}	
TEST(SimulationTime, ParallelLeafSimulationTime) {
	const double time = 0.5;

	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {-1, time, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Leaf(b, p, env, log);
	// EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}

TEST(SimulationTime, ParallelRootSimulationTime) {
	const double time = 0.5;

	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {-1, time, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Root(b, p, env, log);
	// EXPECT_EQ(log.search_count, simulation_count * env.thread_num);

}
TEST(SimulationTime, ParallelTreeSimulationTime) {
	const double time = 0.5;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {-1, time, thread_num, "","",  "openmp"};
	Log log;

	Policy::MCTS_Parallel_Tree(b, p, env, log);
	// EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}

TEST(Simulation, Log) {
	Log l;
	l.printLog();
	Agent a(BLACK);
	board b;
	EnvParameter e;
	a.take_action(b, Policy::MCTS_Serial, l, e);
	cout << b << '\n';
}
GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
