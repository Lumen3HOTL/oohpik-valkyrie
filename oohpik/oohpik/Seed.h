#pragma once
#include "Object.h"

namespace ookpik {
	class Seed :public df::Object {
	private:
		bool m_collected; // true once picked up, so a second collision event in the same move is ignored
	public:
		Seed();

		// Mark this seed as picked up and schedule its removal.
		// Returns true only the first time, so the caller counts each seed once
		bool collect();
		//placeholder for game logic
	};
}