#pragma once
#include "Object.h"
#include "EventKeyboard.h"

class Hero : public df::Object {
private:
    void turn(int delta);
    void forward();
    void updateFrame();

	int m_direction; // 0 = up, 1 = right, 2 = down, 3 = left
    int m_moves; // Move counter

public:
    Hero();
    ~Hero();
    int eventHandler(const df::Event* p_e) override;

};