![](https://github.com/CyCTW/PP-Final_Project/workflows/ParallelMCTS/badge.svg)
[![codecov](https://codecov.io/gh/CyCTW/Parallel-MCTS/branch/master/graph/badge.svg?token=LPORRSHSZT)](https://codecov.io/gh/CyCTW/Parallel-MCTS)

# Parallel Monte Carlo Tree Search
This repo contains the implementation of Parallel Monte Carlo Tree Search with C++.
Three parallel method:
- **Leaf parallelization**
- **Root parallelization**
- **Tree parallelization**

Also, we analyze different methods' performance against each other in [Surakarta](https://en.wikipedia.org/wiki/Surakarta_(game)) Game. 
If you're interested in details, here is the links of our search:
- [Experiment report](https://hackmd.io/2I8fuKY0TY-sRNQU4Dcuog).
- [Slides](https://docs.google.com/presentation/d/11DV9MijsDh2fgR8W5I8kdCT4u630Bibq6ZfMPpffD6U/edit#slide=id.p)

## Development Environment
- Ubuntu 18.04
- GCC 7.5.0
- CMAKE 3.10
- GoogleTest 1.10.0 (latest)

## Requirement
- C++ 17
- OpenMP
- CMAKE (3.10)

## Build
### Construct build directory (optional, but recommened)
```
mkdir build
```
This step create a `build` directory, which build a clean environment for CMake compiling and building.
### Compile file with CMake
```
cd build
cmake ..
```
or
```
cmake .
```
if you don't create `build` directory.
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

   -c  --simCount   <simulationCount>           Set simulation count per step
   -t  --simTime    <simulationTime (second)>   Set simulation time per step
   -T  --threadNum  <threadNum>                 Set number of thread num
   -p  --policy     <blackAndWhitePolicy>       Set Player's policy 
                    <blackPolicy> <whitePolicy> ( choice: Serial, Leaf, Root, Tree. default: Tree Serial)
   -m  --method     <blackAndWhiteMethod>       Set Player's arallel methods 
                    <blackMethod> <whiteMethod> ( choice: openmp, pthread.  default: openmp )
   -?  --help                                   Edit config.h to set parameter
```

### Some Examples:
- Serial version (Black) v.s. Serial version (White) with simulation count $1000$
    ```
    ./mcts -c 1000 -p Serial
    ```
- ParallelTree version (Black) v.s. Serial version (White) with simulation time $2s$
    ```
    ./mcts -t 2 -p Tree Serial
    ```
*Note: we can't set Count and Time simultaneously, choose only one option.*

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with $4$ thread, both use `openmp` implementation.
    ```
    ./mcts -T 4 -p Root Tree -m openmp
    ```

- ParallelRoot version (Black) v.s. ParallelLeaf (White) with $4$ thread. Use `openmp` (Black) and `pthread` (White) implementation.
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
*The first command run test script using Googletest's origin format.*
*The second command run the same test script, but from CMake.*
### Explanation
Mainly, the test script check the Monte Carlo Tree Search's simulation count and simulation time works correctly.


