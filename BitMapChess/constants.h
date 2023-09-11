#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants
{
	/* all the dimensions are in pixels */
	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 512;
	const int BOARD_DIMENSION = 512;
	const int SQUARE_DIMENSION = 64;
}

namespace pieces
{
	/*
	defines for the piece_images_ array to make it easier to access as follows:
	FIRST_SECOND --> COLOR_PIECE
	*/	
	const int B_ROOK = 0;
	const int B_KNIGHT = 1;
	const int B_BISHOP = 2;
	const int B_QUEEN = 3;
	const int B_KING = 4;
	const int B_PAWN = 5;
	
	const int W_ROOK = 6;
	const int W_KNIGHT = 7;
	const int W_BISHOP = 8;
	const int W_QUEEN = 9;
	const int W_KING = 10;
	const int W_PAWN = 11;
}

#define EMPTY 0xFFFFFFFFFFFFFFFF		// used for all legal moves map
#define OCCUPIED 0x8000000000000000		// mask: 0b1000...0000 to check for pieces
#define WHITE -1
#define BLACK 1

#endif // !CONSTANTS_H
