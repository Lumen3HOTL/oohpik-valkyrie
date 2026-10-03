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
			TimerManager(); // P r i v a t e ( a s i n g l e t o n ) .
			TimerManager(TimerManager const&); // Don ’ t a l l o w copy .
			void operator =(TimerManager const&); // Don ’ t a l l o w a s s i g n m e n t
		
			std::vector<Timer> m_timers;
		public:
			static TimerManager& getInstance();

			int startUp();

			// C l o s e g r a p h i c s window .
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