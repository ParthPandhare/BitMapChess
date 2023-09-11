#include "gameHandler.h"

GameHandler* GameHandler::instance_ptr_ = NULL;

void GameHandler::displayBoard()
{
	std::cout << "WHITE PIECES:" << std::endl;
	displayMap(white_piece_map_);
	std::cout << "BLACK PIECES:" << std::endl;
	displayMap(black_piece_map_);
}

void GameHandler::displayPiece(int piece)
{
	displayMap(pieces_[piece]);
}

void GameHandler::displayMap(uint64_t map)
{
	for (int i = 1; i < 9; ++i)
		std::cout << std::bitset<8>(map >> (64 - 8 * i)) << std::endl;
	std::cout << "--------" << std::endl;
}

/* PRIVATE FUNCTIONS */

void GameHandler::generateLegalMoves()
{
	for (int i = 0; i < 12; ++i)
	{
		int team = WHITE;
		switch (i)
		{
		case pieces::B_PAWN:
			team = BLACK;
		case pieces::W_PAWN:
		{
			// iterates through pawn_map & finds all the pawns; for every pawn, it generates the moves & adds them to all_legal_moves_
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					// resets previous moves
					all_legal_moves_[position] = 0;

					// if promoting
					

					// forward moves, including double jump:
					// if there is NOT a piece right infront of the pawn
					if (((all_piece_map_ << position + team * 8) & OCCUPIED) == 0)
					{
						all_legal_moves_[position] |= OCCUPIED >> position + team * 8;

						// same thing but for double jump; checks if it's in starting square && if it can double jump:
						if (position / 8 == 3.5 - 2.5 * team && ((all_piece_map_ << position + team * 16) & OCCUPIED) == 0)
							all_legal_moves_[position] |= OCCUPIED >> position + team * 16;
					}

					// captures
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if it's empty (& we're looking at white moves), set it 
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
			break;
		}
		case pieces::B_BISHOP:
			team = BLACK;
		case pieces::W_BISHOP:
		{

			break;
		}
		case pieces::B_KNIGHT:
			team = BLACK;
		case pieces::W_KNIGHT:
		{

			break;
		}
		case pieces::B_ROOK:
			team = BLACK;
		case pieces::W_ROOK:
		{

			break;
		}
		case pieces::B_QUEEN:
			team = BLACK;
		case pieces::W_QUEEN:
		{

			break;
		}
		case pieces::B_KING:
			team = BLACK;
		case pieces::W_KING:
		{

			break;
		}
		}
	}
}