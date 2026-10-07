#pragma once
#include "Object.h"

class GameOver : public df::Object {
    int m_moves;
    int m_death_input_delay = 30;
public:
    GameOver(int moves);
    int draw() override;
    int eventHandler(const df::Event* p_e) override;
    
};