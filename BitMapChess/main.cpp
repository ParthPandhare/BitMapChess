#include "renderer.h"
#include "gameHandler.h"

static Renderer* renderer = nullptr;
static GameHandler* game = nullptr;

int main(int argc, char* argv[])
{
	/*renderer = Renderer::getInstance();

	while (renderer->isRunning())
	{
		renderer->handleEvents();
		renderer->render();
	}*/

	game = GameHandler::getInstance();
	game->displayMap(game->getAllLegalMoves()[55]);

	return 0;
}