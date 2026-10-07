#pragma once
#include "Object.h"
#include "EventKeyboard.h"

class Hero : public df::Object {
private:
    void turn(int delta);
    void forward();
    void updateFrame();
    void die();

	int m_direction; // 0 = up, 1 = right, 2 = down, 3 = left
    int m_moves; // Move counter
    int m_seeds; // Seeds collected

public:
    Hero();
    ~Hero();
    int eventHandler(const df::Event* p_e) override;
    int draw() override; // the owl, plus the status line on the top row

};