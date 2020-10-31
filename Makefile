all:
	g++ -std=c++17 -O3 main.cpp -o mcts
clean:
	rm mcts