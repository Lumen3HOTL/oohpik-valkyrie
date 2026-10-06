#include "GameOver.h"
#include "EventManager.h"
#include "EventKeyboard.h"
#include "DisplayManager.h"
#include "GameManager.h"

GameOver::GameOver(int moves) {
    setType("GameOver");
    setSolidness(df::SPECTRAL); // can't be collided with
    m_moves = moves;
    df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
}

int GameOver::draw() {
    df::DisplayManager& dm = df::DisplayManager::getInstance(); // not DM: that macro is still wrong
    dm.drawString(df::Vector(40, 10), "you died!", df::CENTER_JUSTIFIED, df::RED);
    dm.drawString(df::Vector(40, 12), "moves: " + std::to_string(m_moves), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(40, 14), "press any key to quit", df::CENTER_JUSTIFIED, df::WHITE);
    return 0;
}

int GameOver::eventHandler(const df::Event* p_e) {
    if (p_e->getType() == df::KEYBOARD_EVENT &&
        static_cast<const df::EventKeyboard*>(p_e)->getKeyboardAction() == df::KEY_PRESSED) {
        GM.setGameOver();
        return 1;
    }
    return 0;
}