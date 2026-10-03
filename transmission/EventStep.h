#pragma once
#include "Event.h"

namespace df {
	const std::string STEP_EVENT = "df::step";
	class EventStep : public Event {

	private:
		unsigned long long m_step_count; // I t e r a t i o n number o f game l o o p .

	public:
		// D e f a u l t c o n s t r u c t o r .
		EventStep();

		// C o n s t r u c t o r w i t h i n i t i a l s t e p c o u n t .
		EventStep(unsigned long long init_step_count);

		// S e t s t e p c o u n t .
		void setStepCount(unsigned long long new_step_count);

		// Get s t e p c o u n t .
		unsigned long long getStepCount() const;
	};
}