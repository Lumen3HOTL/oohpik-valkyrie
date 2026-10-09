#pragma once
#include "Object.h"

class TitleScreen : public df::Object {
private:
    int m_selected;          // highlighted menu option
    int m_pending_action;    // option chosen by a key press, carried out on the next step (-1 = none)
    bool m_showing_controls; // true while the controls guide is on screen
    bool m_showing_scores;   // true while the high score table is on screen

    void choose(int option);
    void carryOut(int option);
    void drawMenu();
    void drawControls();
    void drawHighScores();

public:
    TitleScreen();
    int eventHandler(const df::Event* p_e) override;
    int draw() override;
};
