#pragma once
#include "Object.h"

namespace ookpik {
	// A new enemy steps to a neighbouring tile every this many steps (30 at 30 fps = 1 s).
	// Later levels set a shorter interval (see enemyMoveStepsForLevel in Level.h).
	const int ENEMY_MOVE_STEPS = 30;

	// Wanders the map one tile at a time. Touching it kills the owl, like a tree.
	// It walks over seeds and the exit without collecting or using them.
	class Enemy : public df::Object {
	private:
		int m_move_steps;    // steps since the last move
		int m_move_interval; // steps between moves

	public:
		Enemy();
		int eventHandler(const df::Event* p_e) override;

		// Steps between moves. Values below 1 are ignored (the interval stays as it was)
		void setMoveInterval(int steps);
		int getMoveInterval() const;

		// Step to a random neighbouring tile that's inside the window and has no tree or
		// other enemy on it. Returns false (and stays put) if there's nowhere to go.
		// Called every getMoveInterval() steps; public so tests can move it directly.
		bool moveOnce();
	};
}
