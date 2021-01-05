#include "gtest/gtest.h"
using namespace std;
	TEST(test, negative) {
		EXPECT_EQ(1, 1);
	}	
	TEST(test, policy) {
		EXPECT_EQ(2,2);
	}

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
