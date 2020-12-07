all:
	g++ -std=c++17 -fopenmp -O3 main.cpp -o mcts
clean:
	rm mcts