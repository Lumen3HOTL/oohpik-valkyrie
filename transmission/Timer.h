#pragma once
#include <string>
#include "Clock.h"

namespace df {
	class Timer {
	private:
		Clock m_timeKeeper;
		Clock m_pauseKeeper;
		std::string m_name;
		bool m_loop;
		bool m_running;
		bool m_paused;
		unsigned long long m_wait_time_setting;
		unsigned long long m_current_wait_time;
		
		unsigned long long m_finish_time;
		unsigned long long m_accumulated_pause_time;
	public:
		Timer();
		Timer(std::string name);
		Timer(std::string name, unsigned long long wait_time);
		Timer(std::string name, unsigned long long wait_time,bool loop);
		void start();
		void stop();
		void pause();
		void resume();
		bool getRunning()const;
		bool getPaused()const;
		bool getFinished()const;
		bool getLoop() const;
		unsigned long long getFinishedTime()const;
		unsigned long long getWaitTime()const;
		void setWaitTime(unsigned long long new_wait_time);
		void reset();
		void setLoop(bool should_loop);
		void setName(std::string new_name);
		std::string getName()const;
		bool update();
	};
}