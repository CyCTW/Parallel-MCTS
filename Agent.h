#pragma once
#include "board.h"

class Agent {

public:
    Agent(PIECE piece) : piece(piece) {};

    Pair take_action(board &b, Pair (*Policy)(board&, const PIECE&, const int&)) {
        const int simulation_count = 1000;
        Pair mv = Policy(b, piece, simulation_count);
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