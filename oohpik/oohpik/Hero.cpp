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

	m_moves = 0;
	m_seeds = 0;
	m_maps = 0;
	m_direction = 1;
	updateFrame();
	df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::COLLISION_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
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
		m_statusChangeCount++;
		m_statusChange2 = true;
		return 1;
	} else if (p_e->getType() == df::COLLISION_EVENT) {
		auto* c = static_cast<const df::EventCollision*>(p_e);
		if (c->getObject2()->getType() == "Tree") {
			die();
		} else if (c->getObject2()->getType() == "Seed") {
			// collect() returns false if this seed was already picked up by an earlier event this move
			if (static_cast<ookpik::Seed*>(c->getObject2())->collect()) {
				m_seeds++;
				m_statusChange0 = true;
				m_statusChange1 = true;
				m_statusChangeCount+=2;
				
			}
		} else if (c->getObject2()->getType() == "mapExit") {
			// use() returns false if this exit already fired earlier this move
			if (static_cast<ookpik::MapExit*>(c->getObject2())->use()) {
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
	// + 4 keeps the result positive when turning left from 0
	m_direction = (m_direction + delta + 4) % 4;
	updateFrame();
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

void Hero::die() {
	double run_seconds = m_run_timer.split() / 1000000.0; // the clock counts microseconds
	new GameOver(m_moves, m_seeds, m_maps, run_seconds);  // show the death screen with this run's totals
	WM.markForDelete(this); // remove the owl; deletion happens at the end of this frame
}