#include "gtest/gtest.h"

#include "board.h"
#include "Policy.h"
#include "MCTS.h"
using namespace std;

TEST(SimulationCount, SerialSimulationCount) {
	const int simulation_count = 1000;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, 0, "",""};
	Log log;

	Policy::MCTS_Serial(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count);
}	
TEST(SimulationCount, ParallelLeafSimulationCount) {
	const int simulation_count = 1000;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "",""};
	Log log;

	Policy::MCTS_Parallel_Leaf(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}

TEST(SimulationCount, ParallelRootSimulationCount) {
	const int simulation_count = 1000;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "",""};
	Log log;

	Policy::MCTS_Parallel_Root(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);

}
TEST(SimulationCount, ParallelTreeSimulationCount) {
	const int simulation_count = 1000;
	const int thread_num = 4;
	board b;
	PIECE p = BLACK;
	EnvParameter env = {simulation_count, -1, thread_num, "",""};
	Log log;

	Policy::MCTS_Parallel_Tree(b, p, env, log);
	EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
}
GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
