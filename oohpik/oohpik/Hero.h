#pragma once
#include "Object.h"
#include "EventKeyboard.h"
#include "EventMapGenDone.h"

// While W (or space) is held, the owl hops again every this many steps (6 at 30 fps = 0.2 s)
const int HOLD_REPEAT_STEPS = 6;

class Hero : public df::Object {
private:
    void turn(int delta);
    void forward();
    void countMove();
    void updateFrame();
    void die();

	int m_direction; // 0 = up, 1 = right, 2 = down, 3 = left
    int m_moves; // Move counter
    int m_seeds; // Seeds collected
    int m_maps; // Maps completed (exits reached)

    std::string m_statusSubstring0;
    bool m_statusChange0;
    std::string m_statusSubstring1;
    bool m_statusChange1;
    std::string m_statusSubstring2;
    bool m_statusChange2;
    std::string m_statusSubstring3;
    bool m_statusChange3;
    std::string m_statusSubstring4;
    std::string m_statusSubstring5;
    std::string m_statusSubstring6;
    std::string m_statusSubstring7;

    std::string m_statusSubstring8;
    std::string m_statusSubstring9;
    
    int m_statusChangeCount;

    std::string m_timeString;

    df::Clock m_timer;
    df::Clock m_run_timer; // whole-run time for the high score table; never reset

    bool m_started;

    bool m_dead; // set by die(); the owl is only removed at the end of the frame, so later input that frame is ignored

    bool m_forward_held; // W (or space) is being held down: keep hopping
    int m_hold_steps;    // steps since the last hop while held

    bool m_move_had_sound; // set when a hop picked up a seed or reached the exit, so the plain hop sound is skipped

public:
    Hero();
    ~Hero();
    int eventHandler(const df::Event* p_e) override;
    int draw() override; // the owl, plus the status line on the top row

    // Read-only accessors, used by GameTests
    int getDirection() const { return m_direction; }
    int getMoves() const { return m_moves; }
    int getSeeds() const { return m_seeds; }
    int getMaps() const { return m_maps; }
    bool isForwardHeld() const { return m_forward_held; }
    std::string getStatusLine() const; // the status line as it was last drawn

};