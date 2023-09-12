#include "renderer.h"

Renderer* Renderer::instance_ptr_ = NULL;
static GameHandler* game = nullptr;

void Renderer::init()
{
	if (!SDL_Init(SDL_INIT_EVERYTHING))
	{
		std::cout << "Subsystems Initialized..." << std::endl;

		/* creates window */
		window_ = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, 0);
		assert(window_);		// checks to see if window is created successfully
		std::cout << "Window created..." << std::endl;

		/* creates renderer */
		renderer_ = SDL_CreateRenderer(window_, -1, 0);
		assert(renderer_);		// checks to see if renderer is created successfully
		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);		// background color (in this case white)
		std::cout << "Renderer created..." << std::endl;

		is_running_ = true;
	}
	else
		is_running_ = false;

	loadImages();
	game = GameHandler::getInstance();
}

void Renderer::handleEvents()
{
	SDL_Event event;
	SDL_WaitEvent(&event);
	if (event.type == SDL_QUIT)
		is_running_ = false;
	else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		handleUserInput(WHITE, event);
}

void Renderer::render()
{
	if (is_running_ == false)
		return;

	SDL_RenderClear(renderer_);

	renderBoard();
	displayPieces(game->getPositions());
	renderMap(highlighted_squares_, highlight_);	// renders highlights if any

	SDL_RenderPresent(renderer_);
}

void Renderer::displayPieces(const uint64_t pieces[])
{
	for (int i = 0; i < 12; ++i)
	{
		renderMap(pieces[i], pieces_[i]);
	}
}

/* PRIVATE FUNCTIONS */

void Renderer::loadImages()
{
	SDL_Surface* tmp[16];
	tmp[0] = IMG_Load("assets/board.png");
	tmp[1] = IMG_Load("assets/black/rook.png");
	tmp[2] = IMG_Load("assets/black/knight.png");
	tmp[3] = IMG_Load("assets/black/bishop.png");
	tmp[4] = IMG_Load("assets/black/queen.png");
	tmp[5] = IMG_Load("assets/black/king.png");
	tmp[6] = IMG_Load("assets/black/pawn.png");
	tmp[7] = IMG_Load("assets/white/rook.png");
	tmp[8] = IMG_Load("assets/white/knight.png");
	tmp[9] = IMG_Load("assets/white/bishop.png");
	tmp[10] = IMG_Load("assets/white/queen.png");
	tmp[11] = IMG_Load("assets/white/king.png");
	tmp[12] = IMG_Load("assets/white/pawn.png");
	tmp[13] = IMG_Load("assets/highlight.png");
	tmp[14] = IMG_Load("assets/selected.png");
	tmp[15] = IMG_Load("assets/promotion.png");

	board_ = SDL_CreateTextureFromSurface(renderer_, tmp[0]);
	highlight_ = SDL_CreateTextureFromSurface(renderer_, tmp[13]);
	selected_ = SDL_CreateTextureFromSurface(renderer_, tmp[14]);
	promotion_ = SDL_CreateTextureFromSurface(renderer_, tmp[15]);
	for (int i = 0; i < 12; ++i)
	{
		pieces_[i] = SDL_CreateTextureFromSurface(renderer_, tmp[i + 1]);
	}

	for (int i = 0; i < 16; ++i)
	{
		SDL_FreeSurface(tmp[i]);
	}
}

void Renderer::renderBoard()
{
	SDL_Rect dest_Rect;
	dest_Rect.x = 0;
	dest_Rect.y = 0;
	dest_Rect.h = constants::BOARD_DIMENSION;
	dest_Rect.w = constants::BOARD_DIMENSION;
	SDL_RenderCopy(renderer_, board_, NULL, &dest_Rect);
}

void Renderer::renderMap(uint64_t map, SDL_Texture* const piece)
{
	if (map == 0)
		return;

	for (int y = 0; y < 8; ++y)
	{
		for (int x = 0; x < 8; ++x)
		{
			if ((map & OCCUPIED) != 0)		// if the most significant bit is a 1
			{
				SDL_Rect dest_Rect;
				dest_Rect.x = x * constants::SQUARE_DIMENSION;
				dest_Rect.y = y * constants::SQUARE_DIMENSION;
				dest_Rect.h = constants::SQUARE_DIMENSION;
				dest_Rect.w = constants::SQUARE_DIMENSION;
				SDL_RenderCopy(renderer_, piece, NULL, &dest_Rect);
			}
			map = map << 1;
		}
	}
}

void Renderer::handleUserInput(int team, SDL_Event& event)
{
	int start_pos = event.button.x / constants::SQUARE_DIMENSION + 8 * (event.button.y / constants::SQUARE_DIMENSION);

	// checks to see if the user actually clicked one of their pieces & if it has any legal moves
	if (((game->getTeamMap(team) << start_pos) & OCCUPIED) != 0 && game->getAllLegalMoves()[start_pos] != 0)
	{
		highlighted_squares_ = game->getAllLegalMoves()[start_pos];
		render();
	}
	else
		return;

	// wait for them to un-click
	while (event.type != SDL_MOUSEBUTTONUP)
		SDL_WaitEvent(&event);
	// wait for them to click again
	while (event.type != SDL_MOUSEBUTTONDOWN)
		SDL_WaitEvent(&event);
	if (event.button.button != SDL_BUTTON_LEFT)
	{
		highlighted_squares_ = 0;
		return;
	}

	int end_pos = event.button.x / constants::SQUARE_DIMENSION + 8 * (event.button.y / constants::SQUARE_DIMENSION);

	// if the user clicked a valid square to move to, move the piece; otherwise return
	if (((highlighted_squares_ << end_pos) & OCCUPIED) != 0)
		game->move(team, start_pos, end_pos);

	highlighted_squares_ = 0;
	return;
}