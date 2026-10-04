#pragma once
#include "Event.h"

namespace df {
	const std::string STEP_EVENT = "df::step";
	class EventStep : public Event {

	private:
		unsigned long long m_step_count; // Iteration number of game loop.

	public:
		// Default constructor.
		EventStep();

		// Constructor with initial step count.
		EventStep(unsigned long long init_step_count);

		// Set step count.
		void setStepCount(unsigned long long new_step_count);

		// Get step count.
		unsigned long long getStepCount() const;
	};
}