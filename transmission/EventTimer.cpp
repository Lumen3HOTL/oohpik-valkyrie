#include "EventTimer.h"
namespace df {
	TimerEvent::TimerEvent() {
		this->setType(TIMER_EVENT);
		m_elapsed = 0;
		m_timerName = "undefined_timer";
	}

	

	TimerEvent::TimerEvent(std::string timerName, unsigned long long elapsedMS) {
		this->setType(TIMER_EVENT);
		m_elapsed = elapsedMS;
		m_timerName = timerName;
	}

	void TimerEvent::setTimerName(std::string new_timer_name) {
		m_timerName = new_timer_name;
	}
	std::string TimerEvent::getTimerName()const {
		return m_timerName;
	}
	void TimerEvent::setTimerElapsed(unsigned long long new_timer_elapsedMS) {
		m_elapsed = new_timer_elapsedMS;
	}
	unsigned long long TimerEvent::getTimerElapsed()const {
		return m_elapsed;
	}
}