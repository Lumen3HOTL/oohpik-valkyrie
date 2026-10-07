// Engine includes
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"

// Game includes
#include "Hero.h"
#include "Level.h"

// Function prototypes
void loadResources(void);
void populateWorld(void);

int main(int argc, char* argv[]) {
	df::GameManager& game_manager = df::GameManager::getInstance();

	// Start up the game engine.
	if (game_manager.startUp()) {
		df::LogManager::getInstance().writeLog("Error starting game manager!");
		game_manager.shutDown();
		return 1;
	}

	// Write each log line to disk immediately, so the log survives a crash or hang
	df::LogManager::getInstance().setFlush(true);

	loadResources();
	populateWorld();

	// Run the game
	game_manager.run();

	// Shut everything down
	game_manager.shutDown();
	return 0;
}

void loadResources(void) {
	RM.loadSprite("Resources/Sprites/player.sprite", "hero");
	RM.loadSprite("Resources/Sprites/tree.sprite", "tree");
	RM.loadSprite("Resources/Sprites/seed.sprite", "seed");
	RM.loadSprite("Resources/Sprites/exit.sprite", "exit");
}

void populateWorld(void) {
	// Create the player first; the map builder moves it to its generated start position
	Hero* p_hero = new Hero();

	// Generate the first map (Level.cpp holds the map settings)
	startNewMap(p_hero);
}
