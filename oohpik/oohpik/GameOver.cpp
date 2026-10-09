#include "GameOver.h"
#include "EventManager.h"
#include "EventKeyboard.h"
#include "EventStep.h"
#include "DisplayManager.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "Level.h"
#include "TitleScreen.h"
#include "HighScores.h"
#include <cstdio>

GameOver::GameOver(int moves, int seeds, int maps, double time) {
    setType("GameOver");
    setSolidness(df::SPECTRAL);
    m_moves = moves;
    m_seeds = seeds;
    m_maps = maps;
    m_time = time;

    // Record the run once, when the death screen appears
    m_rank = submitHighScore(ScoreEntry{ seeds, maps, time });
    df::EventManager::getInstance().registerEvent(this, df::KEYBOARD_EVENT);
    df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
}

int GameOver::draw() {
    df::DisplayManager& dm = df::DisplayManager::getInstance(); // not DM: that macro is still wrong

    // Blank panel behind the text so the map doesn't show through. Each drawn character
    // paints its cell's background, and the map (altitude 0) is drawn before this object.
    // Sprites are drawn centred on their position, so the 2-wide map tiles show on
    // odd-even column pairs; columns 31-82 cover whole tiles and never cut one in half.
    const std::string blank_row(52, ' ');
    for (int row = 2; row <= 27; row++) {
        dm.drawString(df::Vector(31, row), blank_row, df::LEFT_JUSTIFIED, df::WHITE);
    }

    dm.drawString(df::Vector(57, 3), "you died!", df::CENTER_JUSTIFIED, df::RED);
    dm.drawString(df::Vector(57, 5), "moves: " + std::to_string(m_moves), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(57, 6), "seeds collected: " + std::to_string(m_seeds), df::CENTER_JUSTIFIED, df::WHITE);
    dm.drawString(df::Vector(57, 7), "maps completed: " + std::to_string(m_maps), df::CENTER_JUSTIFIED, df::WHITE);
    char time_text[32];
    std::snprintf(time_text, sizeof(time_text), "time: %.2fs", m_time);
    dm.drawString(df::Vector(57, 8), time_text, df::CENTER_JUSTIFIED, df::WHITE);

    // Where this run landed in the high score table
    if (m_rank >= 0) {
        dm.drawString(df::Vector(57, 10), "new high score! #" + std::to_string(m_rank + 1), df::CENTER_JUSTIFIED, df::YELLOW);
    } else {
        dm.drawString(df::Vector(57, 10), "next time...", df::CENTER_JUSTIFIED, df::WHITE);
    }
    dm.drawString(df::Vector(57, 12), "high scores", df::CENTER_JUSTIFIED, df::YELLOW);
    drawHighScoreTable(13, m_rank);

    dm.drawString(df::Vector(57, 26), "press any key to return to the title screen", df::CENTER_JUSTIFIED, df::WHITE);
    return 0;
}

int GameOver::eventHandler(const df::Event* p_e) {
    if (p_e->getType() == df::STEP_EVENT) {
        if (m_death_input_delay > 0) {
            m_death_input_delay--;
        }
        // Return to the title one frame after the key press
        if (m_return_pending) {
            m_return_pending = false;
            clearMap();
            new TitleScreen();
            WM.markForDelete(this);
        }
        return 0;
    }
    if (p_e->getType() == df::KEYBOARD_EVENT && static_cast<const df::EventKeyboard*>(p_e)->getKeyboardAction() == df::KEY_PRESSED) {
        if (m_death_input_delay > 0) return 0; // too soon after death
        m_return_pending = true;
        return 1;
    }
    return 0;
}