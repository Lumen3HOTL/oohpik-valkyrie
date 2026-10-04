#include "Hero.h"
#include "WorldManager.h"

Hero::Hero() {
	// initialize general system params
	setType("Hero");
	setSolidness(df::HARD);
	setAltitude(3);

	// initialize position vector
	df::Vector p;

	// how long to wait between moves (in game loop iterations)
	move_slowdown = 5;
}

// Placeholder so the game links; Object's destructor is virtual.
Hero::~Hero() {
}

// Placeholder so the game links; ignores all events for now.
int Hero::eventHandler(const df::Event* p_e) {
	return 0;
}

