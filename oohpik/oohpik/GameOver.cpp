#include "GameOver.h"
#include "EventManager.h"
#include "EventKeyboard.h"
#include "EventStep.h"
#include "DisplayManager.h"
#include "GameManager.h"

GameOver::GameOver(int moves, int seeds, int maps) {
    setType("GameOver");
    setSolidness(df::SPECTRAL); // can't be collided with
    m_moves = moves;
    m_seeds = seeds;
    m_maps = maps;
    df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
    df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
}

int GameOver::draw() {
    df::DisplayManager& dm = df::DisplayManager::getInstance(); // not DM: that macro is still wrong
    dm.drawString(df::Vector(57, 11), "you died!", df::CENTER_JUSTIFIED, df::RED);
    dm.drawString(df::Vector(57, 13), "moves: " + std::to_string(m_moves), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(57, 14), "seeds collected: " + std::to_string(m_seeds), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(57, 15), "maps completed: " + std::to_string(m_maps), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(57, 17), "press any key to quit", df::CENTER_JUSTIFIED, df::WHITE);
    return 0;
}

int GameOver::eventHandler(const df::Event* p_e) {
    if (p_e->getType() == df::STEP_EVENT && m_death_input_delay > 0) {
        m_death_input_delay--;
        return 0;
    }
    if (p_e->getType() == df::KEYBOARD_EVENT && static_cast<const df::EventKeyboard*>(p_e)->getKeyboardAction() == df::KEY_PRESSED) {
        if (m_death_input_delay > 0) return 0; // too soon after death
        GM.setGameOver();
        return 1;
    }
    return 0;
}