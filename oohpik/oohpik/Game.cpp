// Engine includes
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"

// Game includes
#include "Hero.h"
#include "MapBuilder.h"
#include "MapGenConfigObj.h"

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

	// Map layout: fills the 115x30 window like the original ookpik.
	// buildMap draws rows from y = 1, so 27 rows + a 1-tile border on each side = rows 1..29.
	ookpik::MapGenConfig config;
	config.setMapOrigin(df::Vector(0, 0));
	config.setMapWidth(113);
	config.setMapHeight(27);
	config.setMapBorderThickness(1);
	config.setMapObjectWidth(1);
	config.setMapObjectheight(1);
	config.setMapObjectAltitude(0);
	config.setObjectsConstructedPerFrame(4000); // build the whole map in one frame
	config.setTimeoutSeconds(10);
	config.setRandomSeed(0); // 0 = pick a new random seed each run

	// Open structures carved out of the forest
	config.setMinRooms(3);
	config.setMaxRooms(6);
	config.setMinRoomWidth(4);
	config.setMaxRoomWidth(12);
	config.setMinRoomHeight(3);
	config.setMaxRoomHeight(6);
	config.setMinRightAngleLines(4);
	config.setMaxRightAngleLine(8);
	config.setMinRightAngleLineWidth(10);
	config.setMaxRightAngleLineWidth(40);
	config.setMinRightAngleLineHeight(3);
	config.setMaxRightAngleLineHeight(12);
	config.setMinDiagLines(3);
	config.setMaxDiagLine(6);
	config.setMinDiagLineWidth(10);
	config.setMaxDiagLineWidth(30);
	config.setMinDiagLineHeight(3);
	config.setMaxDiagLineHeight(10);

	// Contents, based on the original game's small-map settings
	config.setMinRandTrees(50);
	config.setMaxRandTrees(150);
	config.setMinSeeds(15);
	config.setMaxSeeds(30);

	ookpik::MapBuilder* p_map_builder = new ookpik::MapBuilder();
	p_map_builder->startGenerateMap(config, p_hero);
}