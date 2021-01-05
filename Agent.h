#pragma once
#include "board.h"
#include "Log.h"
#include<chrono>
// #include<time.h>
using namespace std;
class Agent {

public:
    Agent(PIECE piece) : piece(piece), step(0) {};

    Pair take_action(board &b, Pair (*Policy)(board&, const PIECE&,const EnvParameter&), Log &log,  EnvParameter &envParam) {

        auto start = chrono::steady_clock::now();
        Pair mv = Policy(b, piece, envParam);
        auto end = chrono::steady_clock::now();
        auto diff_time = end - start;

        /*** Log Storage ***/
        log.cost_time += chrono::duration <double, milli> (diff_time).count();
        log.search_count += SIMULATION_COUNT;
        /*** Log Storage ***/
        

        EXEC_STATE S = b.move(mv.prev, mv.next, piece);
        step++;
        if (S == FAIL) {
            return {};
        } else {
            return mv;
        }
    };

    PIECE get_piece() const { return piece; }

    int getStep() {
        return step;
    }
protected:
    const PIECE piece;

    int step;

}; 