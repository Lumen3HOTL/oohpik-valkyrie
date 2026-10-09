#pragma once
#include "Object.h"

class GameOver : public df::Object {
    int m_moves;
    int m_seeds; // seeds collected over the whole run
    int m_maps;  // maps completed (exits reached)
    double m_time; // length of the run, in seconds
    int m_rank;    // position in the high score table (0 = best), -1 if it doesnt make the top 10
    int m_death_input_delay = 30;
    bool m_return_pending = false; // a key was pressed; go back to the title screen on the next step
public:
    GameOver(int moves, int seeds, int maps, double time);
    int draw() override;
    int eventHandler(const df::Event* p_e) override;

    // Read-only accessors, used by GameTests
    int getMoves() const { return m_moves; }
    int getSeeds() const { return m_seeds; }
    int getMaps() const { return m_maps; }
    double getTime() const { return m_time; }
    int getRank() const { return m_rank; }

};