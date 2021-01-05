![](https://github.com/CyCTW/PP-Final_Project/workflows/ParallelMCTS/badge.svg)

# Parallel Monte Carlo Tree Search
Implementation of Parallel Monte Carlo Tree Search with C++.
**Three parallel method:**
- Leaf parallelization
- Root parallelization
- Tree parallelization

## Requirement
- C++ 17
- OpenMP
- CMake

## Build
*Preferred way:*
### construct build directory for safety
```
mkdir build
cd build
```
### compile file with CMake
```
cmake ..
```
### build file
```
make
```
This command will generate two execution file 
- `mcts` for executing main function.
- `MCTS_unittest` for unit testing.
### main execution usage
```
Usage: mcts [options]
   -c  --simCount                     Simulation count per step
   -t  --simTime                      Simulation time per step
   -T  --threadNum                    Number of thread num
   -b  --blackPolicy                  Black policy (e.g. Serial, Leaf, Root, Tree)
   -w  --whitePolicy                  White policy (e.g. Serial, Leaf, Root, Tree)
   -?  --help                         Edit config.h to set parameter
```
### test execution usage
```
ctest
```
