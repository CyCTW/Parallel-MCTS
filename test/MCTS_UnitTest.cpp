#include "gtest/gtest.h"

#include "../src/Agent.h"
#include "../src/board.h"
#include "../src/Policy.h"
#include "../src/MCTS.h"
#include "../src/Log.h"
using namespace std;
class  MonteCarloTreeTest : public ::testing::Test {
  protected:
	void SetUp() override {
		p = BLACK;
	}
	void SetSerial() {
		env = {simulation_count, time, thread_num, "", "", "", ""};
	}
	void SetCount(int c) {
		simulation_count = c;
		time = -1;
	}
	void SetTime(double t) {
		time = t;
		simulation_count = -1;
	}
	void SetOMP() {
		method = "openmp";
	}
	void SetPthread() {
		method = "pthread";
	}
	void SetEnv() {
		env = {simulation_count, time, thread_num, "", "", method, method};
	}
	int simulation_count;
	double time;
	const int thread_num = 4;
	PIECE p;
	board b;
	EnvParameter env;
	Log log;
	Agent a;
	string method;

};
/* Test Simulation Count*/
namespace {
	TEST_F(MonteCarloTreeTest, SerialSimulationCount) {
		SetSerial();
		SetCount(100);
		SetEnv();
		a.take_action(b, Policy::MCTS_Serial, log, env);
		EXPECT_EQ(log.search_count, simulation_count);
	}	

	TEST_F(MonteCarloTreeTest, ParallelLeafOMPSimulationCount) {
		SetOMP();
		SetCount(100);
		SetEnv();
		a.take_action(b, Policy::MCTS_Parallel_Leaf, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
	}

	TEST_F(MonteCarloTreeTest, ParallelRootOMPSimulationCount) {
		SetOMP();
		SetCount(100);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Root, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
	}
	TEST_F(MonteCarloTreeTest, ParallelTreeOMPSimulationCount) {
		SetOMP();
		SetCount(100);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Tree, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
	}

	TEST_F(MonteCarloTreeTest, ParallelLeafPthreadSimulationCount) {
		SetPthread();
		SetCount(100);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Leaf, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
	}

	TEST_F(MonteCarloTreeTest, ParallelRootPthreadSimulationCount) {
		SetPthread();
		SetCount(100);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Leaf, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);

	}
	TEST_F(MonteCarloTreeTest, ParallelTreePthreadSimulationCount) {
		SetPthread();
		SetCount(100);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Tree, log, env);
		EXPECT_EQ(log.search_count, simulation_count * env.thread_num);
	}
}

/* Test Simulation Time */
namespace {
	TEST_F(MonteCarloTreeTest, SerialSimulationTime) {
		SetPthread();
		SetTime(5);
		SetEnv();

		a.take_action(b, Policy::MCTS_Serial, log, env);
		double err = 0.5;
		EXPECT_LT(log.cost_time, time + err);
	}	
	TEST_F(MonteCarloTreeTest, ParallelLeafSimulationTime) {
		SetPthread();
		SetTime(5);
		SetEnv();
		
		a.take_action(b, Policy::MCTS_Parallel_Leaf, log, env);
		double err = 0.5;
		EXPECT_LT(log.cost_time, time + err);
	}

	TEST_F(MonteCarloTreeTest, ParallelRootSimulationTime) {
		SetPthread();
		SetTime(5);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Root, log, env);
		double err = 0.5;
		EXPECT_LT(log.cost_time, time + err);
	}

	TEST_F(MonteCarloTreeTest, ParallelTreeSimulationTime) {
		SetPthread();
		SetTime(5);
		SetEnv();

		a.take_action(b, Policy::MCTS_Parallel_Tree, log, env);
		double err = 0.5;
		EXPECT_LT(log.cost_time, time + err);
	}
}

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
