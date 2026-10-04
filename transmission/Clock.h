#pragma once

#include <chrono>

namespace df {
	class Clock {
	private:
		//the previous time delta
		std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_time;
	public:
		// Sets previous time to current time.
		Clock();

		// Return time elapsed since delta() was last called, -1 if error.
		// Resets previous time.
		// Units are microseconds.
		long long delta();

		// Does not reset previous time.
		// Units are microseconds.
		long long split();


	};
	
}