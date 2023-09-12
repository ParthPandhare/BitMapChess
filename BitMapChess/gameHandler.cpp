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

void GameHandler::move(int team, int start_pos, int end_pos)
{
	// move the piece on the piece maps
	for (int i = 0; i < 12; ++i)
	{
		if ((pieces_[i] & OCCUPIED >> start_pos) != 0)
		{
			pieces_[i] ^= OCCUPIED >> start_pos;
			pieces_[i] ^= OCCUPIED >> end_pos;
		}
		else if ((pieces_[i] & OCCUPIED >> start_pos) != 0)
		{
			pieces_[i] &= ~(OCCUPIED >> end_pos);
		}

		// en passants
	}

	// move the piece on the team maps
	if (team == WHITE)
	{
		white_piece_map_ ^= OCCUPIED >> start_pos;
		white_piece_map_ ^= OCCUPIED >> end_pos;
		
		black_piece_map_ &= ~(OCCUPIED >> end_pos);		// handles normal captures 

		// if it's an en-passant 
	}
	else if (team == BLACK)
	{
		black_piece_map_ ^= OCCUPIED >> start_pos;
		black_piece_map_ ^= OCCUPIED >> end_pos;

		white_piece_map_ &= ~(OCCUPIED >> end_pos);		// handles normal captures 

		// if it's an en-passant 
	}

	all_piece_map_ = white_piece_map_ | black_piece_map_;
	generateLegalMoves();
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
					if (!isPieceAtPosition(position + 8 * team))
					{
						all_legal_moves_[position] |= OCCUPIED >> position + team * 8;

						// same thing but for double jump; checks if it's in starting square && if it can double jump:
						if (position / 8 == 3.5 - 2.5 * team && !isPieceAtPosition(position + 16 * team))
							all_legal_moves_[position] |= OCCUPIED >> position + team * 16;
					}

					// captures
					// normal captures:
					if (position % 8 != 7 && isEnemyAtPosition(position + team * 8 + 1, team))
						all_legal_moves_[position] |= OCCUPIED >> position + team * 8 + 1;
					if (position % 8 != 0 && isEnemyAtPosition(position + team * 8 - 1, team))
						all_legal_moves_[position] |= OCCUPIED >> position + team * 8 - 1;

					// en passants:
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
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

bool GameHandler::isPieceAtPosition(int position)
{
	if (((all_piece_map_ << position) & OCCUPIED) == 0)
		return false;
	return true;
}

bool GameHandler::isEnemyAtPosition(int position, int your_team)
{
	if (your_team == WHITE && ((black_piece_map_ << position) & OCCUPIED) == 0)
		return false;
	else if (your_team == BLACK && ((white_piece_map_ << position) & OCCUPIED) == 0)
		return false;
	return true;
}