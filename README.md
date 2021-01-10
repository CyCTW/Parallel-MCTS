![](https://github.com/CyCTW/PP-Final_Project/workflows/ParallelMCTS/badge.svg)
[![codecov](https://codecov.io/gh/CyCTW/Parallel-MCTS/branch/master/graph/badge.svg?token=LPORRSHSZT)](https://codecov.io/gh/CyCTW/Parallel-MCTS)

# Parallel Monte Carlo Tree Search
This repo implements three methods of Parallel Monte Carlo Tree Search with C++.

**The Three parallel methods:**
- Leaf parallelization
- Root parallelization
- Tree parallelization

Also, we analyze different methods' performance against each other in [Surakarta](https://en.wikipedia.org/wiki/Surakarta_(game)) Game. 
If you're interested in details, here are the links to our search:
- [Experiment report](https://hackmd.io/2I8fuKY0TY-sRNQU4Dcuog).
- [Slides](https://docs.google.com/presentation/d/11DV9MijsDh2fgR8W5I8kdCT4u630Bibq6ZfMPpffD6U/edit#slide=id.p)
## Features
- CI pipeline (Autobuild, Autotest)
- Code coverage check
## Structure
```
Parallel-MCTS/
├── build
├── CMakeLists.txt
├── CMakeLists.txt.in
├── LICENSE
├── README.md
├── script
│   ├── cov.sh
│   └── record.sh
├── src
│   ├── Agent.h
│   ├── board.h
│   ├── config.h
│   ├── Log.h
│   ├── main.cpp
│   ├── MCTS.h
│   ├── ParallelRoot.h
│   ├── ParallelTree.h
│   ├── Policy.h
│   └── TreeNode.h
└── unittest
    └── MCTS_UnitTest.cpp

4 directories, 17 files
```
## Development Environment
- Ubuntu 18.04
- GCC 7.5.0
- CMAKE 3.10
- GoogleTest 1.10.0 (latest)

## Requirement
### Must
- C++ 17
- OpenMP
- CMAKE (3.10)
### optional
- lcov 
- gcov 

## Build
### Construct build directory (optional, but recommened)
```
mkdir build
```
This step creates a `build` directory, which builds a clean environment for CMake compiling and building.
### Compile file with CMake
```
cd build
cmake ..
```
or
```
cmake .
```
if you don't create the `build` directory.
### Build file
```
cmake --build .
```
or
```
make
```
This command will generate two execution file 
- `mcts` for executing main function.
- `MCTS_UnitTest` for unit testing.

## Usage
For the specific usage, you can type:
```
./mcts -?
```
for detailed usage.

For example, the command will print
```
Usage: mcts [options]

Note: -c (count) & -t (time) options can't be specified simultaneously.

   -c  --simCount   <simulationCount>           Set simulation count per step.       default: 1000
   -t  --simTime    <simulationTime (second)>   Set simulation time per step.        default: -1
   -T  --threadNum  <threadNum>                 Set number of thread num.            default: 4
   -p  --policy     <blackAndWhitePolicy>       Set Player's policy 
                    <blackPolicy> <whitePolicy>  (options: Serial, Leaf, Root, Tree) default: Tree Serial)

   -m  --method     <blackAndWhiteMethod>       Set Player's arallel methods 
                    <blackMethod> <whiteMethod>  (options: openmp, pthread)          default: openmp)

   -?  --help                                   Help message.
```

### Some quick examples:
- Serial version (Black) v.s. Serial version (White) with simulation count 1000
    ```
    ./mcts -c 1000 -p Serial
    ```
- ParallelTree version (Black) v.s. Serial version (White) with simulation time 2s
    ```
    ./mcts -t 2 -p Tree Serial
    ```
*Note: we can't set Count and Time simultaneously, choose only one option.*

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with 4 thread, both use `openmp` implementation.
    ```
    ./mcts -T 4 -p Root Tree -m openmp
    ```

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with 4 thread. Use `openmp` (Black) and `pthread` (White) implementation.
    ```
    ./mcts -T 4 -p Root Leaf -m openmp pthread
    ```
## UnitTest (GoogleTest)
### command:
```
./MCTS_UnitTest
```
or
```
ctest
```
Both command will run the same unittest script stored in `<ProjectRoot>/unittest/MCTS_UnitTest.cpp`

*Note:*
*The first command runs the test script using Googletest's origin format.*

*The second command runs the same test script, but from CMake.*
### Explanation
Mainly, the test script checks the Monte Carlo Tree Search's simulation count and simulation time works correctly.

## CodeCoverage Report 
If you want to see the code coverage report, you should recompile the project with `-DCOVERAGE=ON` 
*Note: default option is `-DCOVERAGE=OFF` because this option will slow down the whole program.*

First return to build directory.
```
cd <ProjectRoot>/build
```
Then, recompile project with flag `-DCOVERAGE=OFF`
```
cmake -DCOVERAGE=ON ..
```
Rebuild the project
```
cmake --build .
```
Last, move to `script` folder and execute the `cov.sh` script.
```
cd <ProjectRoot>/script
./cov.sh
```
*Note: you should install `lcov` and `gcov` before execute the script.*

If everything works fine, the script will open a html file, which contains the code coverage report.
## Experiment record
If you want to run multiple games on different policys, and record the result for further analysis, you can check our `record.sh`

You can move to `script` directory to check the `record.sh` file.
```
cd <ProjectRoot>/script
```
### usage
```
Usage: ./record.sh [options]

Note: -c (count) & -t (time) options can't be specified simultaneously.

   -c  --simCount   <simulationCount>           Set simulation count per step.       default: 1000
   -t  --simTime    <simulationTime (second)>   Set simulation time per step.        default: -1
   -T  --threadNum  <threadNum>                 Set number of thread num.            default: 4
   -p  --policy     <blackAndWhitePolicy>       Set Player's policy 
                    <blackPolicy> <whitePolicy>  (options: Serial, Leaf, Root, Tree) default: Tree Serial)

   -m  --method     <blackAndWhiteMethod>       Set Player's arallel methods 
                    <blackMethod> <whiteMethod>  (options: openmp, pthread)          default: openmp)

```

### Some quick examples:
- Serial version (Black) v.s. Serial version (White) with simulation count 1000
    ```
    ./record.sh -c 1000 -p Serial
    ```
- ParallelTree version (Black) v.s. Serial version (White) with simulation time 2s
    ```
    ./record.sh -t 2 -p Tree Serial
    ```
*Note: we can't set Count and Time simultaneously, choose only one option.*

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with 4 thread, both use `openmp` implementation.
    ```
    ./record.sh -T 4 -p Root Tree -m openmp
    ```

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with 4 thread. Use `openmp` (Black) and `pthread` (White) implementation.
    ```
    ./record.sh -T 4 -p Root Leaf -m openmp pthread
    ```
