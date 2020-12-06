#pragma once
#include "board.h"
#include "Log.h"
#include<chrono>
// #include<time.h>
using namespace std;
class Agent {

public:
    Agent(PIECE piece) : piece(piece) {};

    Pair take_action(board &b, Pair (*Policy)(board&, const PIECE&, const int&), Log &log) {
        const int simulation_count = 40000;

        auto start = chrono::steady_clock::now();
        Pair mv = Policy(b, piece, simulation_count);
        auto end = chrono::steady_clock::now();
        auto diff_time = end - start;

        /*** Log Storage ***/
        log.cost_time += chrono::duration <double, milli> (diff_time).count();
        log.search_count += simulation_count;
        /*** Log Storage ***/
        

        EXEC_STATE S = b.move(mv.prev, mv.next, piece);

        if (S == FAIL) {
            return {};
        } else {
            return mv;
        }
    };

    PIECE get_piece() const { return piece; }
protected:
    const PIECE piece;

}; 