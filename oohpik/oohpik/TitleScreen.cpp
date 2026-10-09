#include "TitleScreen.h"
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "EventManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "Hero.h"
#include "Level.h"
#include <string>

namespace {
	enum MenuOption { PLAY, CONTROLS, QUIT, OPTION_COUNT };
	const char* MENU_LABELS[OPTION_COUNT] = { "[P]lay", "[C]ontrols", "[Q]uit" };

	const int CENTER_X = 57; // middle of the 115 unit wide window

	// Owl art from the original ookpik.py
	const char* OWL_ART[] = {
		R"ART(__________-------____                 ____-------__________)ART",
		R"ART(\------____-------___--__---------__--___-------____------/)ART",
		R"ART( \//////// / / / / / \   _-------_   / \ \ \ \ \ \\\\\\\\/)ART",
		R"ART(   \////-/-/------/_/_| /___   ___\ |_\_\------\-\-\\\\/)ART",
		R"ART(     --//// / /  /  //|| (O)\ /(O) ||\\  \  \ \ \\\\--)ART",
		R"ART(          ---__/  // /| \_  /V\  _/ |\ \\  \__---)ART",
		R"ART(               -//  / /\_ ------- _/\ \  \\-)ART",
		R"ART(                 \_/_/ /\---------/\ \_\_/)ART",
		R"ART(                     ----\   |   /----)ART",
		R"ART(                          | -|- |)ART",
		R"ART(                         /   |   \)ART",
		R"ART(                         ---- \___|)ART",
	};
	const int OWL_ART_LINES = sizeof(OWL_ART) / sizeof(OWL_ART[0]);
	const int OWL_ART_WIDTH = 59;
}

TitleScreen::TitleScreen() {
	setType("TitleScreen");
	setSolidness(df::SPECTRAL);
	m_selected = PLAY;
	m_pending_action = -1;
	m_showing_controls = false;
	df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
	df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
}

int TitleScreen::eventHandler(const df::Event* p_e) {
	// Wait one frame to carry out events so the key event doesn't also happen in-game
	if (p_e->getType() == df::STEP_EVENT) {
		if (m_pending_action != -1) {
			int option = m_pending_action;
			m_pending_action = -1;
			carryOut(option);
		}
		return 1;
	}

	// keyboard event handler
	if (p_e->getType() == df::KEYBOARD_EVENT) {
		auto* k = static_cast<const df::EventKeyboard*>(p_e);
		if (k->getKeyboardAction() != df::KEY_PRESSED) return 0;

		// Any key leaves the controls guide
		if (m_showing_controls) {
			m_showing_controls = false;
			return 1;
		}

		switch (k->getKey()) {
			case df::Keyboard::W: case df::Keyboard::UPARROW:
				m_selected = (m_selected + OPTION_COUNT - 1) % OPTION_COUNT;
				break;
			case df::Keyboard::S: case df::Keyboard::DOWNARROW:
				m_selected = (m_selected + 1) % OPTION_COUNT;
				break;
			case df::Keyboard::RETURN: case df::Keyboard::SPACE: choose(m_selected); break;
			case df::Keyboard::P: choose(PLAY); break;
			case df::Keyboard::C: choose(CONTROLS); break;
			case df::Keyboard::Q: case df::Keyboard::ESCAPE: choose(QUIT); break;
			default: return 0;
		}
		return 1;
	}
	return 0;
}

void TitleScreen::choose(int option) {
	m_selected = option;
	m_pending_action = option;
}

void TitleScreen::carryOut(int option) {
	switch (option) {
		case PLAY: {
			// Create the owl and the first map; map builder process handles initial owl position
			Hero* p_hero = new Hero();
			startNewMap(p_hero);
			WM.markForDelete(this);
			break;
		}
		case CONTROLS:
			m_showing_controls = true;
			break;
		case QUIT:
			GM.setGameOver();
			break;
	}
}

int TitleScreen::draw() {
	if (m_showing_controls) {
		drawControls();
	} else {
		drawMenu();
	}
	return 0;
}

void TitleScreen::drawMenu() {
	df::DisplayManager& dm = df::DisplayManager::getInstance();

	// Art lines are drawn left-aligned from one column so the picture keeps its shape
	int art_left = CENTER_X - OWL_ART_WIDTH / 2;
	for (int i = 0; i < OWL_ART_LINES; i++) {
		dm.drawString(df::Vector(art_left, 2 + i), OWL_ART[i], df::LEFT_JUSTIFIED, df::WHITE);
	}
	dm.drawString(df::Vector(CENTER_X, 15), "o o k p i k", df::CENTER_JUSTIFIED, df::YELLOW);

	for (int i = 0; i < OPTION_COUNT; i++) {
		std::string label = MENU_LABELS[i];
		df::Color color = df::WHITE;
		if (i == m_selected) {
			label = "> " + label + " <";
			color = df::YELLOW;
		}
		dm.drawString(df::Vector(CENTER_X, 18 + 2 * i), label, df::CENTER_JUSTIFIED, color);
	}

	dm.drawString(df::Vector(CENTER_X, 26), "w/s or arrow keys to choose, enter to select", df::CENTER_JUSTIFIED, df::WHITE);
}

void TitleScreen::drawControls() {
	df::DisplayManager& dm = df::DisplayManager::getInstance();

	dm.drawString(df::Vector(CENTER_X, 4), "controls", df::CENTER_JUSTIFIED, df::YELLOW);

	// Key list, left-aligned so the descriptions line up
	int keys_left = CENTER_X - 18;
	dm.drawString(df::Vector(keys_left, 7), "w or space    hop forward one tile", df::LEFT_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(keys_left, 8), "a             turn left", df::LEFT_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(keys_left, 9), "d             turn right", df::LEFT_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(keys_left, 10), "esc           quit", df::LEFT_JUSTIFIED, df::WHITE);

	dm.drawString(df::Vector(CENTER_X, 13), "you are the owl (^ > v <), pointing the way you face.", df::CENTER_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(CENTER_X, 14), "you can only hop forward one tile, or turn on the spot.", df::CENTER_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(CENTER_X, 16), "collect the seeds (@) and avoid the trees (#): flying into one is fatal.", df::CENTER_JUSTIFIED, df::WHITE);
	dm.drawString(df::Vector(CENTER_X, 17), "reach the exit (E) to move on to a new forest.", df::CENTER_JUSTIFIED, df::WHITE);

	dm.drawString(df::Vector(CENTER_X, 22), "press any key to return", df::CENTER_JUSTIFIED, df::WHITE);
}
