// Engine includes
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"

// Game includes
#include "Hero.h"
#include "Tree.h"

// Function prototypes
void loadResources(void);
void populateWorld(void);

int main(int argc, char* argv[]) {
	df::GameManager& game_manager = df::GameManager::getInstance();

	// Start up the game engine
	if (game_manager.startUp()) {
		df::LogManager::getInstance().writeLog("Error starting game manager!");
		game_manager.shutDown();
		return 1;
	}

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
}

void populateWorld(void) {
	// Create player
	new Hero();

	// Add obstacles
	for (int x = 35; x <= 45; x++) new Tree(df::Vector(x, 8));
}