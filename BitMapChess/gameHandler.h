#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include <iostream>
#include <bitset>
#include "SDL.h"
#include "constants.h"

class GameHandler
{
public:
	// deleting copy constructor
	GameHandler(const GameHandler& obj)
		= delete;

	// returns the single instance of this class
	static GameHandler* getInstance()
	{
		if (instance_ptr_ == NULL)
		{
			instance_ptr_ = new GameHandler();
			return instance_ptr_;
		}
		else
		{
			return instance_ptr_;
		}
	}

	void displayBoard();			// for debugging; prints out team bitmaps
	void displayPiece(int piece);	// for debugging; prints out piece bitmaps
	void displayMap(uint64_t map);	// for debugging; prints out a given map

	uint64_t* getPositions() { return pieces_; }
	uint64_t* getAllLegalMoves() { return all_legal_moves_; }
	uint64_t getTeamMap(int team) { if (team == WHITE) return white_piece_map_; return black_piece_map_; }

	void move(int team, int start_pos, int end_pos);


private:
	static GameHandler* instance_ptr_;
	GameHandler()
	{
		all_piece_map_ = 0xFFFF00000000FFFF;
		white_piece_map_ = 0x000000000000FFFF;
		black_piece_map_ = 0xFFFF000000000000;
		pieces_[pieces::B_BISHOP] = 0x2400000000000000;
		pieces_[pieces::B_KNIGHT] = 0x4200000000000000;
		pieces_[pieces::B_ROOK] = 0x8100000000000000;
		pieces_[pieces::B_QUEEN] = 0x1000000000000000;
		pieces_[pieces::B_KING] = 0x0800000000000000;
		pieces_[pieces::B_PAWN] = 0x00FF000000000000;
		pieces_[pieces::W_BISHOP] = 0x0000000000000024;
		pieces_[pieces::W_KNIGHT] = 0x0000000000000042;
		pieces_[pieces::W_ROOK] = 0x0000000000000081;
		pieces_[pieces::W_QUEEN] = 0x0000000000000010;
		pieces_[pieces::W_KING] = 0x0000000000000008;
		pieces_[pieces::W_PAWN] = 0x000000000000FF00;
		for (int i = 0; i < 64; ++i)
			all_legal_moves_[i] = 0x00;

		generateLegalMoves();
	}

	void generateLegalMoves();		// populates all_legal_moves_ with the current maps
	bool isPieceAtPosition(int position);
	bool isEnemyAtPosition(int position, int your_team);
	void generateDiagonalMoves(int position, int team);
	void generateStraightMoves(int position, int team);

	/*
	- piece_map_ is a bitmap storing the positions of all existing pieces on the board (1 if there is a piece, 0 otherwise)
	- pieces_ is an array of bitmaps, 1 for each piece, portraying where each piece is on the board
	- all legal moves shows the possible positions a piece (if it exists) at a certain position can go to. If a square is empty, it's filled with all 1s
	*/

	uint64_t white_piece_map_, black_piece_map_, all_piece_map_;
	uint64_t pieces_[12];
	uint64_t all_legal_moves_[64];
};

#endif // !GAME_HANDLER_H
