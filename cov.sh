cd build/
ctest
cd CMakeFiles/MCTS_UnitTest.dir/test
gcov MCTS_UnitTest.cpp.gcno
lcov --capture --directory . --output-file gtest_coverage.info
genhtml gtest_coverage.info --output-directory CODE_COVERAGE
cd CODE_COVERAGE
xdg-open index.html
