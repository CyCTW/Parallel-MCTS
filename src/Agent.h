#pragma once
#include "board.h"
#include "Log.h"

#include <chrono>

using namespace std;
class Agent {

public:
    Agent() : piece(BLACK) , step(0) {};
    Agent(PIECE piece) : piece(piece), step(0) {};

    Pair take_action(board &b, Pair (*Policy)(board&, const PIECE&,const EnvParameter&, Log &log), Log &log,  EnvParameter &envParam) {

        Pair mv = Policy(b, piece, envParam, log);

        EXEC_STATE S = b.move(mv.prev, mv.next, piece);
        step++;
        if (S == FAIL) {
            return {};
        } else {
            return mv;
        }
    };
    inline int getStep() const {  return step; }
    
protected:
    const PIECE piece;
    int step;

}; 