#pragma once
#include "Object.h"

class GameOver : public df::Object {
    int m_moves;
    int m_seeds; // seeds collected over the whole run
    int m_maps;  // maps completed (exits reached)
    int m_death_input_delay = 30;
    bool m_return_pending = false; // a key was pressed; go back to the title screen on the next step
public:
    GameOver(int moves, int seeds, int maps);
    int draw() override;
    int eventHandler(const df::Event* p_e) override;
    
};