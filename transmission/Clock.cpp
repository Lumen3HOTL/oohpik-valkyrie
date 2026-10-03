#include "Clock.h"
#include <chrono>
namespace df {

	Clock::Clock() {
		m_previous_time = std::chrono::high_resolution_clock::now();
	}

	//same logic as the code in the book, just with modern c++ primitives
	long long Clock::delta() {
		std::chrono::time_point<std::chrono::high_resolution_clock> newTime = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(newTime - m_previous_time);
		m_previous_time = newTime;
		return elapsed.count();
	}
	//same logic as the code in the book, just with modern c++ primitives
	long long Clock::split() {
		std::chrono::time_point<std::chrono::high_resolution_clock> newTime = std::chrono::high_resolution_clock::now();
		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(newTime - m_previous_time);
		return elapsed.count();
	}
}