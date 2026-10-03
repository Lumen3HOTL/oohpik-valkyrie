#include "EventStep.h"

namespace df {
	EventStep::EventStep() {
		m_step_count = 0ull;
		this->setType(STEP_EVENT);
	}
	EventStep::EventStep(unsigned long long init_step_count) {
		m_step_count = init_step_count;
		this->setType(STEP_EVENT);
	}
	

	void EventStep::setStepCount(unsigned long long new_step_count) {
		m_step_count = new_step_count;
	}

	unsigned long long EventStep::getStepCount() const {
		return m_step_count;
	}
}