#pragma once
#include <string>
#include <vector>
#include "Timer.h"
#include "EventTimer.h"
#include "Manager.h"
#include "WorldManager.h"

namespace df {
	class TimerManager :public Manager {
		private:
			TimerManager(); // Private (a singleton).
			TimerManager(TimerManager const&); // Don't allow copy.
			void operator =(TimerManager const&); // Don't allow assignment
		
			std::vector<Timer> m_timers;
		public:
			static TimerManager& getInstance();

			int startUp();

			// Close graphics window.
			void shutDown();

			int update();

			Timer* getTimer(std::string timer_name);

			int addTimer(std::string timer_name);

			int addTimer(std::string timer_name,unsigned long long timer_setting, bool looping=false);

			int removeTimer(std::string timer_name);

			int getCount()const;

			bool isEmpty()const;

			int clear();

	};
}