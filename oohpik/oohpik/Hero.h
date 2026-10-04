#pragma once
#include "Object.h"

class Hero : public df::Object {
private:
    //void kbd(const df::EventKeyboard* p_keyboard_event);
    void move(int dy);
    void step();

    int move_slowdown;
    int move_countdown = move_slowdown;

public:
    Hero();
    ~Hero();
    int eventHandler(const df::Event* p_e) override;

};