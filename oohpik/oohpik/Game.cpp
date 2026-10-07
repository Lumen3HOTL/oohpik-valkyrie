// Engine includes
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"

// Function prototypes.
void loadResources(void);

int main(int argc, char* argv[]) {
	df::GameManager& game_manager = df::GameManager::getInstance();

	// Start up the game engine.
	if (!game_manager.startUp()) {
		df::LogManager::getInstance().startUp();
		df::LogManager::getInstance().writeLog("Error starting game manager!");
		game_manager.shutDown();
		return 1;
	}

	loadResources();

	// Run the game (this blocks until the game ends).
	game_manager.run();

	// Shut everything down.
	game_manager.shutDown();
	return 0;
}

void loadResources(void) {

}
