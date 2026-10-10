// Engine includes
#include "GameManager.h"
#include "ResourceManager.h"
#include "LogManager.h"

// Game includes
#include "TitleScreen.h"
#include "GameTests.h"

#include <cstring>

// Function prototypes
void loadResources(void);

int main(int argc, char* argv[]) {
	// "oohpik.exe --test" runs the game tests (GameTests.cpp) instead of the game
	if (argc > 1 && std::strcmp(argv[1], "--test") == 0) {
		return runGameTests();
	}
#ifdef _DEBUG
	// "oohpik.exe --mapstress" builds 200 maps back to back and counts any that never finish
	if (argc > 1 && std::strcmp(argv[1], "--mapstress") == 0) {
		return runMapStress(200);
	}
	// "oohpik.exe --mapscan" checks maps from seeds 1-400 for seeds or an exit the owl can't reach
	if (argc > 1 && std::strcmp(argv[1], "--mapscan") == 0) {
		return runMapScan(1, 400);
	}
	// "oohpik.exe --frameprofile" times each part of the frame loop on a full map
	if (argc > 1 && std::strcmp(argv[1], "--frameprofile") == 0) {
		return runFrameProfile(60);
	}
#endif

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

	// Start on the title screen
	new TitleScreen();

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

	// The engine doesn't log sound loading failures, report them here
	const char* sounds[][2] = {
		{ "Resources/Sounds/move.wav", "move" },
		{ "Resources/Sounds/getseed.wav", "getseed" },
		{ "Resources/Sounds/nextlevel.wav", "nextlevel" },
		{ "Resources/Sounds/death.wav", "death" },
	};
	for (const auto& sound : sounds) {
		if (RM.loadSound(sound[0], sound[1]) != 0) {
			df::LogManager::getInstance().writeLog("Error loading sound %s from %s", sound[1], sound[0]);
		}
	}
	if (RM.loadMusic("Resources/Sounds/outside.wav", "outside") != 0) {
		df::LogManager::getInstance().writeLog("Error loading music outside from Resources/Sounds/outside.wav");
	}
}
