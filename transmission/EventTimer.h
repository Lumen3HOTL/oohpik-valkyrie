#pragma once
#include <string>
#include "event.h"

namespace df {
	const std::string TIMER_EVENT = "df::Timer";
	class TimerEvent : public Event {

	private:
		unsigned long long m_elapsed;
		std::string m_timerName;

	public:
		// D e f a u l t c o n s t r u c t o r .
		TimerEvent();

		// C o n s t r u c t o r w i t h i n i t i a l s t e p c o u n t .
		TimerEvent(std::string timerName, unsigned long long elapsedMS);

		void setTimerName(std::string new_timer_name);
		std::string getTimerName()const;
		void setTimerElapsed(unsigned long long new_timer_elapsedMS);
		unsigned long long getTimerElapsed()const;
	};
}