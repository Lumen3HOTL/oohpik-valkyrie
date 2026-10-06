#include "Hero.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventManager.h"
#include "GameManager.h"
#include "EventCollision.h"
#include "GameOver.h"

Hero::Hero() {
	// initialize general system params
	setType("Hero");
	setSolidness(df::HARD);
	setAltitude(3);
	setSprite("hero");

	// initialize position vector
	df::Vector p(40, 12);
	setPosition(p);

	m_moves = 0;
	m_direction = 1;
	updateFrame();
	df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::COLLISION_EVENT);
}

// Placeholder so the game links; Object's destructor is virtual.
Hero::~Hero() {
}

// Handle events, primarily movement
int Hero::eventHandler(const df::Event* p_e) {
	if (p_e->getType() == df::KEYBOARD_EVENT) {
		auto* k = static_cast<const df::EventKeyboard*>(p_e);
		if (k->getKeyboardAction() != df::KEY_PRESSED) return 0; // one action per press, like input()
		switch (k->getKey()) {
			case df::Keyboard::W: case df::Keyboard::SPACE: forward(); break;
			case df::Keyboard::A: turn(-1); break;
			case df::Keyboard::D: turn(+1); break;
			case df::Keyboard::ESCAPE: GM.setGameOver(); break;
			default: return 0;
		}
		m_moves++;
		return 1;
	} else if (p_e->getType() == df::COLLISION_EVENT) {
		auto* c = static_cast<const df::EventCollision*>(p_e);
		if (c->getObject2()->getType() == "Tree") {
			die();
		}
		return 1;
	}
	return 0;
}

// Turn in place. delta is -1 for left, +1 for right
void Hero::turn(int delta) {
	// + 4 keeps the result positive when turning left from 0
	m_direction = (m_direction + delta + 4) % 4;
	updateFrame();
}

// Hop one cell in the facing direction.
void Hero::forward() {
	// Movement per direction: 0 = up, 1 = right, 2 = down, 3 = left
	const int dx[] = { 0, 1, 0, -1 };
	const int dy[] = { -1, 0, 1, 0 };

	// Work out where we would land
	df::Vector p = getPosition();
	df::Vector target(p.getX() + dx[m_direction], p.getY() + dy[m_direction]);

	// Block at the window edge
	df::DisplayManager& display_manager = df::DisplayManager::getInstance();
	if (target.getX() < 0 || target.getX() >= display_manager.getHorizontal() ||
		target.getY() < 0 || target.getY() >= display_manager.getVertical()) {
		return;
	}

	// Move
	WM.moveObject(this, target);
	updateFrame();
}

void Hero::updateFrame() {
	df::Animation a = getAnimation();
	a.setIndex(m_direction);
	a.setSlowdownCount(-1); // -1 stops the animation so the frame stays on our facing
	setAnimation(a);
}

void Hero::die() {
	new GameOver(m_moves);  // show the death screen
	WM.markForDelete(this); // remove the owl; deletion happens at the end of this frame
}