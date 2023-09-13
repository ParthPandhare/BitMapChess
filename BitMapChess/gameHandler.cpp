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

bool GameHandler::move(int team, int start_pos, int end_pos)
{
	// move the piece on the piece maps && deletes the captured piece, if any, on that piece's map
	int moved_piece;
	for (int i = 0; i < 12; ++i)
	{
		if ((pieces_[i] & OCCUPIED >> start_pos) != 0)
		{
			pieces_[i] ^= OCCUPIED >> start_pos;
			pieces_[i] ^= OCCUPIED >> end_pos;
			moved_piece = i;
		}
		else if ((pieces_[i] & OCCUPIED >> end_pos) != 0)
		{
			pieces_[i] &= ~(OCCUPIED >> end_pos);
		}
	}
	// en passant captures on piece maps
	// if the moved piece is a pawn, it moved diagonally, and there's nothing on the end_pos in the other team's map, en passant
	if (moved_piece == pieces::W_PAWN && end_pos % 8 != start_pos % 8 && ((black_piece_map_ >> end_pos) & OCCUPIED) == 0)
	{
		black_piece_map_ &= ~(OCCUPIED >> end_pos + 8);
		pieces_[pieces::B_PAWN] &= ~(OCCUPIED >> end_pos + 8);
	}
	else if (moved_piece == pieces::B_PAWN && end_pos % 8 != start_pos % 8 && ((white_piece_map_ >> end_pos) & OCCUPIED) == 0)
	{
		white_piece_map_ &= ~(OCCUPIED >> end_pos - 8);
		pieces_[pieces::W_PAWN] &= ~(OCCUPIED >> end_pos - 8);
	}

	// move the piece on the team maps
	if (team == WHITE)
	{
		white_piece_map_ ^= OCCUPIED >> start_pos;
		white_piece_map_ ^= OCCUPIED >> end_pos;
		
		black_piece_map_ &= ~(OCCUPIED >> end_pos);		// handles normal captures 
	}
	else if (team == BLACK)
	{
		black_piece_map_ ^= OCCUPIED >> start_pos;
		black_piece_map_ ^= OCCUPIED >> end_pos;

		white_piece_map_ &= ~(OCCUPIED >> end_pos);		// handles normal captures 
	}

	all_piece_map_ = white_piece_map_ | black_piece_map_;

	// if a pawn is promoting
	bool promoting = false;
	if ((team == WHITE && end_pos / 8 == 0 && ((pieces_[pieces::W_PAWN] << end_pos) & OCCUPIED) != 0) || 
		(team == BLACK && end_pos / 8 == 7 && ((pieces_[pieces::B_PAWN] << end_pos) & OCCUPIED) != 0))
		promoting = true;

	// setting en passants for the next move
	if ((moved_piece == pieces::W_PAWN && start_pos / 8 - end_pos / 8 == 2) || (moved_piece == pieces::B_PAWN && start_pos / 8 - end_pos / 8 == -2))
		en_passantable_piece_ = end_pos;

	generateLegalMoves();
	turn_ *= -1;
	return promoting;
}

void GameHandler::promote(int position, int piece, int team)
{
	pieces_[piece] |= OCCUPIED >> position;

	if (team == WHITE)
		pieces_[pieces::W_PAWN] &= ~(OCCUPIED >> position);
	else if (team == BLACK)
		pieces_[pieces::B_PAWN] &= ~(OCCUPIED >> position);

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
					generatePawnMoves(position, team);
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
			// iterates through bishop map, finds all bishops, and generates the legal moves for each piece
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					all_legal_moves_[position] = 0;
					generateDiagonalMoves(position, team);
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
			break;
		}
		case pieces::B_KNIGHT:
			team = BLACK;
		case pieces::W_KNIGHT:
		{
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					all_legal_moves_[position] = 0;
					generateKnightMoves(position, team);
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
			break;
		}
		case pieces::B_ROOK:
			team = BLACK;
		case pieces::W_ROOK:
		{
			// iteratres through rook maps, finds all rooks, & generates the legal moves for each piece
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					all_legal_moves_[position] = 0;
					generateStraightMoves(position, team);
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
			break;
		}
		case pieces::B_QUEEN:
			team = BLACK;
		case pieces::W_QUEEN:
		{
			// same as rook & bishop, but combined
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					all_legal_moves_[position] = 0;
					generateStraightMoves(position, team);
					generateDiagonalMoves(position, team);
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
			break;
		}
		case pieces::B_KING:
			team = BLACK;
		case pieces::W_KING:
		{
			for (int position = 0; position < 64; ++position)
			{
				if (((pieces_[i] << position) & OCCUPIED) != 0)
				{
					all_legal_moves_[position] = 0;
					generateKingMoves(position, team);
					break;
				}
				else if (((all_piece_map_ << position) & OCCUPIED) == 0) 	// if the position is empty, set it to empty
				{
					all_legal_moves_[position] = EMPTY;
				}
			}
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

void GameHandler::generateDiagonalMoves(int position, int team)
{
	int x_incr = 1;
	int y_incr = 1;

	// going to the right
	if (position % 8 != 7)
	{
		// generating moves going down & to the right
		while ((position + x_incr) % 8 <= 7 && (position + 8 * y_incr) / 8 <= 7)
		{
			int possible_move = position + x_incr + 8 * y_incr;

			if (!isPieceAtPosition(possible_move))
				all_legal_moves_[position] |= OCCUPIED >> possible_move;
			else if (isEnemyAtPosition(possible_move, team))
			{
				all_legal_moves_[position] |= OCCUPIED >> possible_move;
				break;
			}
			else
				break;

			if ((position + x_incr) % 8 == 7)
				break;

			x_incr++;
			y_incr++;
		}
		if (position / 8 != 0)
		{
			x_incr = 1;
			y_incr = 1;
			// generating moves going up & to the right
			while ((position + x_incr) % 8 <= 7 && (position - 8 * y_incr) / 8 >= 0)
			{
				int possible_move = position + x_incr - 8 * y_incr;

				if (!isPieceAtPosition(possible_move))
					all_legal_moves_[position] |= OCCUPIED >> possible_move;
				else if (isEnemyAtPosition(possible_move, team))
				{
					all_legal_moves_[position] |= OCCUPIED >> possible_move;
					break;
				}
				else
					break;

				if ((position + x_incr) % 8 == 7)
					break;

				x_incr++;
				y_incr++;
			}
		}
	}

	// going to the left
	if (position % 8 != 0)
	{
		x_incr = 1;
		y_incr = 1;
		// generating moves going down & to the left
		while ((position - x_incr) % 8 >= 0 && (position + 8 * y_incr) / 8 <= 7)
		{
			int possible_move = position - x_incr + 8 * y_incr;

			if (!isPieceAtPosition(possible_move))
				all_legal_moves_[position] |= OCCUPIED >> possible_move;
			else if (isEnemyAtPosition(possible_move, team))
			{
				all_legal_moves_[position] |= OCCUPIED >> possible_move;
				break;
			}
			else
				break;

			if ((position - x_incr) % 8 == 0)
				break;

			x_incr++;
			y_incr++;
		}
		if (position / 8 != 0)
		{
			x_incr = 1;
			y_incr = 1;
			// generating moves going up & to the left
			while ((position - x_incr) % 8 >= 0 && (position - 8 * y_incr) / 8 >= 0)
			{
				int possible_move = position - x_incr - 8 * y_incr;

				if (!isPieceAtPosition(possible_move))
					all_legal_moves_[position] |= OCCUPIED >> possible_move;
				else if (isEnemyAtPosition(possible_move, team))
				{
					all_legal_moves_[position] |= OCCUPIED >> possible_move;
					break;
				}
				else
					break;

				if ((position - x_incr) % 8 == 0)
					break;

				x_incr++;
				y_incr++;
			}
		}
	}
}

void GameHandler::generateStraightMoves(int position, int team)
{
	int horizontal_position = position % 8;

	// horizontally right moves:
	for (int i = 0; i < 7 - horizontal_position; )
	{
		++i;
		if (!isPieceAtPosition(position + i))
			all_legal_moves_[position] |= OCCUPIED >> position + i;
		else if (isEnemyAtPosition(position + i, team))
		{
			all_legal_moves_[position] |= OCCUPIED >> position + i;
			break;
		}
		else break;
	}

	// horizontally left moves:
	for (int i = 0; i < horizontal_position; )
	{
		++i;
		if (!isPieceAtPosition(position - i))
			all_legal_moves_[position] |= OCCUPIED >> position - i;
		else if (isEnemyAtPosition(position - i, team))
		{
			all_legal_moves_[position] |= OCCUPIED >> position - i;
			break;
		}
		else break;
	}

	// all up & down moves; same basic logic as the horizontal moves
	int vertical_position = position / 8;

	// vertical down moves:
	for (int i = 0; i < 7 - vertical_position; )
	{
		++i;
		if (!isPieceAtPosition(position + i * 8))
			all_legal_moves_[position] |= OCCUPIED >> position + i * 8;
		else if (isEnemyAtPosition(position + i * 8, team))
		{
			all_legal_moves_[position] |= OCCUPIED >> position + i * 8;
			break;
		}
		else break;
	}

	// vertical up moves:
	for (int i = 0; i < vertical_position; )
	{
		++i;
		if (!isPieceAtPosition(position - i * 8))
			all_legal_moves_[position] |= OCCUPIED >> position - i * 8;
		else if (isEnemyAtPosition(position - i * 8, team))
		{
			all_legal_moves_[position] |= OCCUPIED >> position - i * 8;
			break;
		}
		else break;
	}
}

void GameHandler::generateKnightMoves(int position, int team)
{
	// knight move generation: brute forces each position after checking bounds
	int x_pos = position % 8;
	int y_pos = position / 8;

	// bottom right square moves
	if (x_pos <= 5 && y_pos <= 5)
	{
		if (!isPieceAtPosition(position + 10) || isEnemyAtPosition(position + 10, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 10;
		if (!isPieceAtPosition(position + 17) || isEnemyAtPosition(position + 17, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 17;
	}
	else if (x_pos <= 5 && y_pos == 6)
	{
		if (!isPieceAtPosition(position + 10) || isEnemyAtPosition(position + 10, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 10;
	}
	else if (y_pos <= 5 && x_pos == 6)
	{
		if (!isPieceAtPosition(position + 17) || isEnemyAtPosition(position + 17, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 17;
	}

	// bottom left square moves
	if (x_pos >= 2 && y_pos <= 5)
	{
		if (!isPieceAtPosition(position + 6) || isEnemyAtPosition(position + 6, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 6;
		if (!isPieceAtPosition(position + 15) || isEnemyAtPosition(position + 15, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 15;
	}
	else if (x_pos >= 2 && y_pos == 6)
	{
		if (!isPieceAtPosition(position + 6) || isEnemyAtPosition(position + 6, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 6;
	}
	else if (y_pos <= 5 && x_pos == 1)
	{
		if (!isPieceAtPosition(position + 15) || isEnemyAtPosition(position + 15, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 15;
	}

	// top right square moves
	if (x_pos <= 5 && y_pos >= 2)
	{
		if (!isPieceAtPosition(position - 6) || isEnemyAtPosition(position - 6, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 6;
		if (!isPieceAtPosition(position - 15) || isEnemyAtPosition(position - 15, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 15;
	}
	else if (x_pos <= 5 && y_pos == 1)
	{
		if (!isPieceAtPosition(position - 6) || isEnemyAtPosition(position - 6, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 6;
	}
	else if (y_pos >= 2 && x_pos == 6)
	{
		if (!isPieceAtPosition(position - 15) || isEnemyAtPosition(position - 15, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 15;
	}

	// top left square moves
	if (x_pos >= 2 && y_pos >= 2)
	{
		if (!isPieceAtPosition(position - 10) || isEnemyAtPosition(position - 10, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 10;
		if (!isPieceAtPosition(position - 17) || isEnemyAtPosition(position - 17, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 17;
	}
	else if (x_pos >= 2 && y_pos == 1)
	{
		if (!isPieceAtPosition(position - 10) || isEnemyAtPosition(position - 10, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 10;
	}
	else if (y_pos >= 2 && x_pos == 1)
	{
		if (!isPieceAtPosition(position - 17) || isEnemyAtPosition(position - 17, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 17;
	}
}

void GameHandler::generateKingMoves(int position, int team)
{
	int x_pos = position % 8;
	int y_pos = position / 8;

	// top right
	if (x_pos != 7 && y_pos != 0)
	{
		if (!isPieceAtPosition(position - 7) || isEnemyAtPosition(position - 7, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 7;
	}

	// top 
	if (y_pos != 0)
	{
		if (!isPieceAtPosition(position - 8) || isEnemyAtPosition(position - 8, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 8;
	}

	// top left
	if (x_pos != 0 && y_pos != 0)
	{
		if (!isPieceAtPosition(position - 9) || isEnemyAtPosition(position - 9, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 9;
	}

	// right
	if (x_pos != 7)
	{
		if (!isPieceAtPosition(position + 1) || isEnemyAtPosition(position + 1, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 1;
	}

	// left
	if (x_pos != 0)
	{
		if (!isPieceAtPosition(position - 1) || isEnemyAtPosition(position - 1, team))
			all_legal_moves_[position] |= OCCUPIED >> position - 1;
	}

	// bottom right
	if (x_pos != 0 && y_pos != 7)
	{
		if (!isPieceAtPosition(position + 9) || isEnemyAtPosition(position + 9, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 9;
	}

	// bottom 
	if (y_pos != 7)
	{
		if (!isPieceAtPosition(position + 8) || isEnemyAtPosition(position + 8, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 8;
	}

	// bottom left
	if (x_pos != 7 && y_pos != 7)
	{
		if (!isPieceAtPosition(position + 7) || isEnemyAtPosition(position + 7, team))
			all_legal_moves_[position] |= OCCUPIED >> position + 7;
	}
}

void GameHandler::generatePawnMoves(int position, int team)
{
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
	if (en_passantable_piece_ != -1 && position / 8 == 3.5 + 0.5 * team)
	{
		if (position + 1 == en_passantable_piece_)	// if there is an en-passantable pawn to the right
		{
			all_legal_moves_[position] |= OCCUPIED >> position + team * 8 + 1;
		}
		else if (position - 1 == en_passantable_piece_)		// if there's an en-passantable pawn to the left
		{
			all_legal_moves_[position] |= OCCUPIED >> position + team * 8 - 1;
		}
	}
}