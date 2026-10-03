#include "Timer.h"

namespace df {
	Timer::Timer() {
		m_timeKeeper=Clock();
		m_pauseKeeper=Clock();
		std::string m_name="undefined_timer";
		m_loop=false;
		m_running=false;
		m_paused=false;
		m_wait_time_setting=0;
		m_current_wait_time=0;
	
		m_finish_time=0;
		m_accumulated_pause_time = 0;
	}
	Timer::Timer(std::string name) {
		m_timeKeeper = Clock();
		m_pauseKeeper = Clock();
		std::string m_name = name;
		m_loop = false;
		m_running = false;
		m_paused = false;
		m_wait_time_setting = 0;
		m_current_wait_time = 0;

		m_finish_time = 0;
		m_accumulated_pause_time = 0;
	}
	Timer::Timer(std::string name, unsigned long long wait_time) {
		m_timeKeeper = Clock();
		m_pauseKeeper = Clock();
		std::string m_name = name;
		m_loop = false;
		m_running = false;
		m_paused = false;
		m_wait_time_setting = wait_time;
		m_current_wait_time = 0;
	
		m_finish_time = 0;
		m_accumulated_pause_time = 0;
	}
	Timer::Timer(std::string name, unsigned long long wait_time, bool loop) {
		m_timeKeeper = Clock();
		m_pauseKeeper = Clock();
		std::string m_name = name;
		m_loop = loop;
		m_running = false;
		m_paused = false;
		m_wait_time_setting = wait_time;
		m_current_wait_time = 0;
		
		m_finish_time = 0;
		m_accumulated_pause_time = 0;
	}
	void Timer::start() {
		this->reset();
		m_current_wait_time = m_wait_time_setting;
		m_running = true;
	}
	void Timer::stop() {
		m_running = false;
		m_paused = false;
		unsigned long long tempFinish = m_timeKeeper.delta();
		m_finish_time = (tempFinish / 1000) - m_accumulated_pause_time;
		
	}
	void Timer::pause() {
		if (m_running) {
			if (!m_paused) {
				m_paused = true;
				m_pauseKeeper.delta();
			}
			
		}
	}
	void Timer::resume() {
		if (m_running) {
			if (m_paused) {
				m_paused = false;
				unsigned long long pauseTime = m_pauseKeeper.delta() / 1000;
				m_accumulated_pause_time += pauseTime;
				m_current_wait_time += pauseTime;

			}
		}
	}
	bool Timer::getRunning()const {
		return m_running;
	}
	bool Timer::getPaused()const {
		return m_paused;
	}
	bool Timer::getFinished()const {
		return ((m_finish_time != 0) && (!m_running));
	}

	bool Timer::getLoop()const {
		return m_loop;
	}
	unsigned long long Timer::getFinishedTime()const {
		return m_finish_time;
	}
	unsigned long long Timer::getWaitTime()const {
		return m_wait_time_setting;
	}
	void Timer::setWaitTime(unsigned long long new_wait_time) {
		this->reset();
		m_wait_time_setting = new_wait_time;
		m_current_wait_time = m_wait_time_setting;
	}
	void Timer::reset() {
		m_timeKeeper.delta();
		m_pauseKeeper.delta();
		
		
		m_running = false;
		m_paused = false;
		
		m_current_wait_time = 0;
	
		m_finish_time = 0;
		m_accumulated_pause_time = 0;
	}
	void Timer::setLoop(bool should_loop) {
		m_loop = should_loop;
	}
	void Timer::setName(std::string new_name) {
		m_name = new_name;
	}
	std::string Timer::getName()const {
		return m_name;
	}

	bool Timer::update() {
		if (m_running) {
			if (m_paused) {
				return false;
			}
			unsigned long long currentTimeStamp = m_timeKeeper.split();

			if (currentTimeStamp >= m_current_wait_time) {
				if (m_loop) {
					unsigned long long tempFinish = m_timeKeeper.delta();
					m_finish_time = (tempFinish / 1000) - m_accumulated_pause_time;
					m_accumulated_pause_time = 0;
					m_current_wait_time = m_wait_time_setting - (m_finish_time - m_wait_time_setting);
					return true;
				}
				this->stop();
				return true;
			}
		}
		return false;
	}
}