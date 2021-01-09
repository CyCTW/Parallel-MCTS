/*
Explanation:
Surakarta Game board, implement functions related to board.
*/

#pragma once

#include "config.h"

#include <array>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <algorithm>
#include <random>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>
#include <iostream>

// #include <torch/torch.h>

class Pair {
public:
	char prev;
	char next;
	Pair() : prev(-1), next(-1) {}
	Pair (char x, char y) : prev(x), next(y) {}

	bool operator==(const Pair &p) {
		return (p.prev == prev && p.next==next);
	}
	bool operator!=(const Pair &p) {
		return ( !((*this)==p) );
	}
	bool operator<(const Pair &p) const {
		return (prev == p.prev ? next < p.next : prev > p.prev);
	}
};


enum PIECE {
	BLACK,
	WHITE,
	SPACE = 9,
	UNKNOWN = -1
};
enum WIN_STATE {
	BLACK_WIN,
	WHITE_WIN,
	DRAW
};
enum EXEC_STATE {
	SUCCESS = 1,
	FAIL = -1
};
enum DIRECTION {
	LEFT = -1,
	RIGHT = 1,
	UP = -2,
	DOWN = 2
};
enum ACTION {
	MOVE,
	EAT
};

const std::array<DIRECTION, 4> Directions{LEFT, RIGHT, UP, DOWN};


class board {

public:
	typedef std::array<PIECE, 6> row;
	typedef std::array<row, 6> grid;
private:
	grid tile;
	
	std::vector<Pair> step_stack[2];
	static const std::size_t stack_limit = 5;
	int step;//if odd, white's turn if even, black's turn
	
public:
	static constexpr int SIZE { 36 };
	static constexpr int COL { 6 };
	static constexpr int repeat_move_limit {6};

	static constexpr std::array<int, 4> corner_pos {0, 5, 30, 35};
	// std::map<char, std::pair<char, std::function< EXEC_STATE(char&, bool , const bool&, char&)> > >mapping_circle;
	static std::unordered_map<char, std::pair<char, DIRECTION > >mapping_circle; // declare below


	void init_board() {
		
		for (int i{0}; i < COL; ++i) {
			for (int j{0}; j < COL; ++j) {
				if (i <= 1) tile[i][j] = BLACK;
				if (i <= 3 && i > 1) tile[i][j] = SPACE;
				else if (i <= 5 && i > 3) tile[i][j] = WHITE;
			}
		}
	}

	board() : tile(), step(0) {
		// init_map();	
		init_board();
	}

	PIECE& operator()(char i)  { return tile[ i / 6 ][ i % 6 ]; }
	PIECE operator()(char i) const {return tile[ i / 6 ][ i % 6 ]; }

	PIECE& operator()(char x, char y)  { return tile[x][y]; }
	PIECE operator()(char x, char y) const { return tile[x][y]; }	

	board& operator =(const board &b) = default;

	// compare pieces on board
	inline WIN_STATE compare_piece() const {
		unsigned b_num = 0, w_num = 0;
		b_num = count_piece(BLACK);
		w_num = count_piece(WHITE);

		if (b_num == w_num)
			return DRAW;
		return (b_num > w_num) ? BLACK_WIN : WHITE_WIN;
	}

	inline PIECE take_turn() const {
		if (step % 2){
			return WHITE;
		} 
		else {
			return BLACK;
		}
	}

	inline unsigned count_piece(const PIECE &piece) const {
		unsigned total = 0;
		for (int i=0; i<SIZE; i++)
			if ( operator()(i) == piece)
				++total;
		
		return total;
	}
private:
	void move_a_step(char &pos, DIRECTION dir) {
		switch (dir) {
			case LEFT : pos--; break;
			case RIGHT: pos++; break;
			case UP   : pos-=COL; break;
			case DOWN : pos+=COL; break;
		}
	}

	bool detect_edge(char &pos, DIRECTION dir) {
		switch (dir) {
			case LEFT : return ( (pos % 6) != 5);
			case RIGHT: return ( (pos % 6) != 0);
			case UP   : return (pos >= 0);
			case DOWN : return (pos < 36);
			default: throw "UNKNOWN DIRECTION";
		}
	}

	EXEC_STATE search (char &pos, bool pass, const PIECE &piece, char &count_step, DIRECTION dir) {

		do {
			count_step++;
			
			// search has passed through circle and can't find eatable piece 
			// or collided with same color piece
			if (count_step >= 25 || (*this)(pos) == piece) {
				count_step = 0;
				return FAIL;
			}
		
			if( (*this)(pos) == (!piece)) {
				// pass: search route has passed a ring or not
				if(pass)
					return SUCCESS;
				else {
					count_step = 0;
					return FAIL;
				}
			}
			move_a_step(pos, dir);
		} while( detect_edge(pos, dir) );
		
		// return to previous position
		move_a_step(pos, static_cast<DIRECTION>(-static_cast<int>(dir) ) );
		
		// pass through circles
		if ( mapping_circle.find(pos) != mapping_circle.end() ){
			pass = true;
			char old_pos = pos;
			pos = mapping_circle[pos].first;
			return search( pos, pass, piece, count_step, mapping_circle[old_pos].second); 
		}
		else {
			count_step = 0;
			return FAIL;
		}
	}

	std::vector<char> check_eat (const char &origin_pos, const PIECE &piece) {
		char pos = origin_pos;
		const char prev_pos = pos;
		char count_step = 0;

		std::vector<char> eatable;	
		
		// corner pos is not on any track, can't eat any piece
		for (auto &p : corner_pos)
			if(p == pos)
				return {};
		
		// search four different ways
		(*this)(pos) = SPACE;

		for (auto &dir : Directions) {
			if ( search(pos, false, piece, count_step, dir) != FAIL) eatable.push_back(pos);	  
			pos = prev_pos; count_step = 0;
		}
		
		(*this)(prev_pos) = piece;

		if (eatable.empty())
			return {};
		else{
			return eatable;  // return ramdomly
		}
	}
	// check whether the moves are available
	// NEW!! Add new rules: no return moves after 3 times.
	std::vector<char> check_move (const char &pos, const PIECE &piece)  {
		std::vector<char> movable;
		
		// 8 directions
		char dir[8] {-7, -6, -5, -1, 1, 5, 6, 7};
		const char no_move {0};

		// pos in top row
		if (pos <= 5 && pos >= 0) 
			dir[0] = dir[1] = dir[2] = no_move;
		
		// pos in left column
		if ( (pos % 6) == 0) 
			dir[0] = dir[3] = dir[5] = no_move;
		
		// pos in right column
		if ( ( (pos+1) % 6) == 0) 
			dir[2] = dir[4] = dir[7] = no_move;
		
		// pos in bottom row
		if (pos <= 35 && pos >= 30) 
			dir[5] = dir[6] = dir[7] = no_move;
		
		for (auto &d : dir) {
			if (d == no_move) continue;
			auto next_pos = (*this)(pos + d);
			if ( next_pos == SPACE ){
				//std::cerr << int(pos+d) << ' ' << int(forbidden_pos) << '\n';
				movable.push_back(pos + d);
			}
		}
		return movable;
	}
public:
	std::vector<Pair> find_piece(const PIECE &piece, ACTION act) {
		std::vector<Pair> eatable;

		for (char now_pos {0}; now_pos < SIZE; ++now_pos) {
			char tile = (*this)(now_pos);
			
			if (tile == piece) {
				std::vector<char> eat_pos;
				if (act == MOVE)
					eat_pos = { check_move(now_pos, piece) };
				else if (act == EAT)
					eat_pos = { check_eat(now_pos, piece) };

				if (!eat_pos.empty()) {
					for (auto &next_pos : eat_pos) 
						eatable.push_back( {now_pos, next_pos} );
				}
				
			}
		}
		return eatable;
	}

public:
	std::vector<Pair> get_available_move(const PIECE &piece) {
		std::vector<Pair> mv = find_piece(piece, MOVE);
		std::vector<Pair> ea = find_piece(piece, EAT);
		ea.insert(ea.end(), mv.begin(), mv.end());
		
		return ea;
	}


	// move or eat a piece
	EXEC_STATE move(const char prev_pos, const char place_pos, const PIECE &piece) {
		if (place_pos >= SIZE || place_pos < 0) return FAIL;

		(*this)(place_pos) = piece;
		(*this)(prev_pos) = SPACE;

		step++;

		return SUCCESS;
	}

public:
/*
                   0 1 2 3 4 5
               ┏━━━━━━━┓ ┏━━━━━━━┓  
               ┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃
              0┃ ┃ 1 1 1 1 1 1 ┃ ┃ 
              1┃ ┗ 1 1 1 1 1 1 ┛ ┃
              2┗━━ 0 0 0 0 0 0 ━━┛
              3┏━━ 0 0 0 0 0 0 ━━┓
              4┃ ┏ 2 2 2 2 2 2 ┓ ┃
              5┃ ┃ 2 2 2 2 2 2 ┃ ┃
               ┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃
               ┗━━━━━━━┛ ┗━━━━━━━┛
            
*/
	friend std::ostream& operator<<(std::ostream& out, const board& b) {
		out << "+--------------------+" << '\n';
		out << "      0 1 2 3 4 5    " << '\n';
		out << "  ┏━━━━━━━┓ ┏━━━━━━━┓" << '\n';
		out << "  ┃ ┏━━━┓ ┃ ┃ ┏━━━┓ ┃" << '\n';
		
		int idx = 0;
		for (auto &row : b.tile) {
			switch(idx) {
				case 0: out << "0 ┃ ┃"; break;
				case 1: out << "1 ┃ ┗"; break;
				case 2: out << "2 ┗━━"; break;
				case 3: out << "3 ┏━━"; break;
				case 4: out << "4 ┃ ┏"; break;
				case 5: out << "5 ┃ ┃"; break;
			}

			for (auto &t : row) {
				if (t==SPACE)
					out << std::setw(2) << 'x';
				else
					out << std::setw(2) << static_cast<int>(t);
			}

			switch(idx) {
				case 0: out << " ┃ ┃"; break;
				case 1: out << " ┛ ┃"; break;
				case 2: out << " ━━┛"; break;
				case 3: out << " ━━┓"; break;
				case 4: out << " ┓ ┃"; break;
				case 5: out << " ┃ ┃"; break;
			}
			out << '\n';
			idx++;
		}
		out << "  ┃ ┗━━━┛ ┃ ┃ ┗━━━┛ ┃" << '\n';
		out << "  ┗━━━━━━━┛ ┗━━━━━━━┛" << '\n';
		out << "+--------------------+" << '\n';
		return out;
	}

};
std::unordered_map<char, std::pair<char, DIRECTION> > board::mapping_circle = 
{
		{ 1,  { 6, RIGHT} }, { 2, {12, RIGHT} }, {3 , {17, LEFT } }, {4 , {11, LEFT } },
		{ 6,  { 1, DOWN } }, {12, { 2, DOWN } }, {18, {32, UP   } }, {24, {31, UP   } },
		{31,  {24, RIGHT} }, {32, {18, RIGHT} }, {33, {23, LEFT } }, {34, {29, LEFT } },
		{29,  {34, UP   } }, {23, {33, UP   } }, {17,  {3, DOWN } }, {11, { 4, DOWN } }
};