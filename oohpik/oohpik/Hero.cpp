#include "Hero.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventManager.h"
#include "GameManager.h"
#include "EventCollision.h"
#include "GameOver.h"
#include "Seed.h"
#include "MapExit.h"
#include "Level.h"
#include <string>
#include "EventStep.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Music.h"

namespace {
	// Play a sound loaded in Game.cpp's loadResources(); does nothing if it failed to load
	void playSound(const std::string& label) {
		df::Sound* p_sound = RM.getSound(label);
		if (p_sound != nullptr) {
			p_sound->play();
		}
	}

	// The in-game music; null if it failed to load
	df::Music* gameMusic() {
		return RM.getMusic("outside");
	}
}

Hero::Hero() {
	// initialize general system params
	setType("Hero");
	setSolidness(df::HARD);
	setAltitude(3);
	setSprite("hero");

	// initialize position vector
	df::Vector p(40, 12);
	setPosition(p);
	m_statusSubstring0 = "seeds: ";
	m_statusSubstring2 = "    remaining: ";
	m_statusSubstring4 = "    moves: ";
	m_statusSubstring6 = "    maps: ";
	m_statusSubstring1 = "";
	m_statusSubstring3 = "";
	m_statusSubstring5 = "";
	m_statusSubstring7 = "";
	m_statusSubstring8 = " second(s)";
	m_statusSubstring9 = "    Level Time: ";
	// Start with every number flagged as changed so the status line shows 0s from the first frame
	m_statusChange0 = true;
	m_statusChange1 = true;
	m_statusChange2 = true;
	m_statusChange3 = true;
	m_timeString = "";
	m_statusChangeCount = 4;

	m_timer = df::Clock();
	m_run_timer = df::Clock();

	m_started = false;
	m_dead = false;

	m_moves = 0;
	m_seeds = 0;
	m_maps = 0;
	m_direction = 1;
	m_move_had_sound = false;
	updateFrame();
	df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::COLLISION_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
	df::EventManager::getInstance().registerEvent(this, ookpik::GEN_DONE_EVENT); // restarts Level Time on each new map

	// A new owl means a run is starting: start the in-game music (loops until the owl dies)
	if (gameMusic() != nullptr) {
		gameMusic()->play(true);
	}
}

// Placeholder so the game links; Object's destructor is virtual.
Hero::~Hero() {
}

// Handle events, primarily movement
int Hero::eventHandler(const df::Event* p_e) {
	// A dead owl stays in the world until the end of the frame; it mustn't move or die again
	if (m_dead && (p_e->getType() == df::KEYBOARD_EVENT || p_e->getType() == df::COLLISION_EVENT)) {
		return 0;
	}
	if (p_e->getType() == df::KEYBOARD_EVENT) {
		auto* k = static_cast<const df::EventKeyboard*>(p_e);
		if (k->getKeyboardAction() != df::KEY_PRESSED) return 0; // one action per press, like input()
		// Moves are counted by turn() and forward() themselves, so a hop the window edge blocks
		// isn't counted, and a fatal hop is counted before die() records the run
		switch (k->getKey()) {
			case df::Keyboard::W: case df::Keyboard::SPACE: forward(); break;
			case df::Keyboard::A: turn(-1); break;
			case df::Keyboard::D: turn(+1); break;
			case df::Keyboard::ESCAPE: GM.setGameOver(); break; // quitting isn't a move
			default: return 0;
		}
		return 1;
	} else if (p_e->getType() == df::COLLISION_EVENT) {
		auto* c = static_cast<const df::EventCollision*>(p_e);
		if (c->getObject2() == nullptr) {
			return 0; // nothing was hit
		}
		if (c->getObject2()->getType() == "Tree") {
			die();
		} else if (c->getObject2()->getType() == "Seed") {
			// collect() returns false if this seed was already picked up by an earlier event this move
			if (static_cast<ookpik::Seed*>(c->getObject2())->collect()) {
				playSound("getseed");
				m_move_had_sound = true;
				m_seeds++;
				m_statusChange0 = true;
				m_statusChange1 = true;
				m_statusChangeCount+=2;
				
			}
		} else if (c->getObject2()->getType() == "mapExit") {
			// use() returns false if this exit already fired earlier this move
			if (static_cast<ookpik::MapExit*>(c->getObject2())->use()) {
				playSound("nextlevel");
				m_move_had_sound = true;
				m_maps++;
				m_statusChangeCount++;
				m_statusChange3 = true;
				startNewMap(this); // old map is removed at the end of this frame
			}
		}
		return 1;
	}
	else if (p_e->getType().compare(ookpik::GEN_DONE_EVENT) == 0) {
		m_timer.delta();
	}
	return 0;
}

// Turn in place. delta is -1 for left, +1 for right
void Hero::turn(int delta) {
	countMove();
	// + 4 keeps the result positive when turning left from 0
	m_direction = (m_direction + delta + 4) % 4;
	updateFrame();
}

// Add one to the move counter and refresh it on the status line
void Hero::countMove() {
	m_moves++;
	m_statusChangeCount++;
	m_statusChange2 = true;
}

// Hop one cell in the facing direction.
void Hero::forward() {
	// Movement per direction: 0 = up, 1 = right, 2 = down, 3 = left
	const int dx[] = { 0, 2, 0, -2 };
	const int dy[] = { -1, 0, 1, 0 };

	// Work out where we would land
	df::Vector p = getPosition();
	df::Vector target(p.getX() + dx[m_direction], p.getY() + dy[m_direction]);

	// Block at the window edge
	df::DisplayManager& display_manager = df::DisplayManager::getInstance();
	if (target.getX() < 0 || target.getX() >= display_manager.getHorizontal() ||
		target.getY() < 0 || target.getY() >= display_manager.getVertical()) {
		return; // blocked: not a move
	}
	countMove(); // before moving, so a fatal hop is in the total die() records

	// Move. moveObject returns -1 if a tree blocked player (collision handler plays death sound),
	// move into a seed or the exit plays that sound instead of the hop sound.
	m_move_had_sound = false;
	if (WM.moveObject(this, target) == 0 && !m_move_had_sound) {
		playSound("move");
	}
	updateFrame();
}

void Hero::updateFrame() {
	df::Animation a = getAnimation();
	a.setIndex(m_direction);
	a.setSlowdownCount(-1); // -1 stops the animation so the frame stays on our facing
	setAnimation(a);
}

// Draw the owl, then the status line on the top row (the map starts at row 1)
int Hero::draw() {
	int result = df::Object::draw();

	if (m_statusChangeCount > 0) {
		if (m_statusChange0) {
			m_statusSubstring1= std::to_string(m_seeds);
		}
		if (m_statusChange1) {
			m_statusSubstring3 = std::to_string(WM.objectsOfTypeCount("Seed"));
		}
		if (m_statusChange2) {
			m_statusSubstring5 = std::to_string(m_moves);
		}
		if (m_statusChange3) {
			m_statusSubstring7=std::to_string(m_maps);
		}
	}
		
	int currentPos = 1;
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring0, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring0.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring1, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring1.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring2, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring2.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring3, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring3.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring4, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring4.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring5, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring5.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring6, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring6.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring7, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring7.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring9, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring9.length();
	m_timeString = std::to_string((((double)((double)((double)(m_timer.split() / 1000)) / 33) / 30)));
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_timeString, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_timeString.length();
	df::DisplayManager::getInstance().drawString(df::Vector(currentPos, 0), m_statusSubstring8, df::LEFT_JUSTIFIED, df::WHITE);
	currentPos += m_statusSubstring8.length();
	return result;
}

// Same pieces, in the same order, that draw() puts on the top row
std::string Hero::getStatusLine() const {
	return m_statusSubstring0 + m_statusSubstring1 + m_statusSubstring2 + m_statusSubstring3 +
		m_statusSubstring4 + m_statusSubstring5 + m_statusSubstring6 + m_statusSubstring7 +
		m_statusSubstring9 + m_timeString + m_statusSubstring8;
}

void Hero::die() {
	if (m_dead) {
		return; // already dead this frame (e.g. a second collision event from the same hop)
	}
	m_dead = true;

	// The run is over: stop the in-game music (the death screen is a menu) and play the death sound
	if (gameMusic() != nullptr) {
		gameMusic()->stop();
	}
	playSound("death");

	double run_seconds = m_run_timer.split() / 1000000.0; // the clock counts microseconds
	new GameOver(m_moves, m_seeds, m_maps, run_seconds);  // show the death screen with this run's totals
	WM.markForDelete(this); // remove the owl; deletion happens at the end of this frame
}