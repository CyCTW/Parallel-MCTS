#!/bin/sh
PROJECT_ROOT=$(pwd | rev | cut -d / -f 1 --complement | rev )
PROJECT_ROOT=$PROJECT_ROOT/build

cd $PROJECT_ROOT

# execute unittest
ctest

cd $PROJECT_ROOT/CMakeFiles/MCTS_UnitTest.dir/uinttest
gcov MCTS_UnitTest.cpp.gcno
lcov --capture --directory . --output-file gtest_coverage.info
genhtml gtest_coverage.info --output-directory CODE_COVERAGE
cd CODE_COVERAGE
xdg-open index.html
