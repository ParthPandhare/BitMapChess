#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>
#include <cassert>
#include "SDL.h"
#include "SDL_image.h"
#include "constants.h"
#include "gameHandler.h"

class Renderer
{
public:
	// deleting copy constructor
	Renderer(const Renderer& obj)
		= delete;

	// returns the single instance of this class
	static Renderer* getInstance()
	{
		if (instance_ptr_ == NULL)
		{
			instance_ptr_ = new Renderer();
			return instance_ptr_;
		}
		else
		{
			return instance_ptr_;
		}
	}

	void render();
	void handleEvents();
	bool isRunning() { return is_running_; }
	void displayPieces(const uint64_t pieces[]);

private:
	static Renderer* instance_ptr_;
	Renderer()
		: promoting_(false, -1)
	{
		renderer_ = nullptr;
		window_ = nullptr;
		board_ = nullptr;
		highlight_ = nullptr;
		selected_ = nullptr;
		promotion_ = nullptr;
		for (int i = 0; i < 12; ++i)
			pieces_[i] = nullptr;
		is_running_ = false;
		highlighted_squares_ = 0;
		init();
	}

	void loadImages();
	void renderBoard();
	void renderMap(uint64_t map, SDL_Texture* const piece); 
	void renderPromotion();
	void init();
	void handleUserInput(int team, SDL_Event& event);
	void handlePromotion();

	bool is_running_;
	std::pair<bool, int> promoting_;
	uint64_t highlighted_squares_;

	SDL_Renderer* renderer_;
	SDL_Window* window_;
	SDL_Texture* board_;
	SDL_Texture* highlight_;
	SDL_Texture* selected_;
	SDL_Texture* promotion_;
	SDL_Texture* pieces_[12];
};

#endif // !RENDERER_H
