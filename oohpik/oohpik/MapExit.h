#pragma once
#include "Object.h"

namespace ookpik {
	class MapExit : public df::Object {
	private:
		bool m_used; // true once the owl has gone through, so a second collision event is ignored
	public:
		MapExit();

		// Mark the exit as used. Returns true only the first time,
		// so one visit starts exactly one new map.
		bool use();
		//placeholder for game logic
	};
}