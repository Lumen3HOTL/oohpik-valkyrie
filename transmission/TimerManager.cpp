#include "TimerManager.h"

namespace df {
	TimerManager::TimerManager() {
		this->setType("TimerManager");
		m_timers = std::vector<Timer>();
	}


	TimerManager& TimerManager::getInstance() {
		static TimerManager TimerMan = TimerManager();
		return TimerMan;
	}

	int TimerManager::startUp() {
		if (this->isStarted()) {
			return -1;
		}
		m_timers.clear();
		return Manager::startUp();
	}

	// Close graphics window.
	void TimerManager::shutDown() {
		m_timers.clear();
		Manager::shutDown();
	}

	int TimerManager::update() {
		if (this->isStarted()) {
			if (this->isEmpty()) {
				return 0;
			}
			WorldManager& wm = WorldManager::getInstance();
			for (int index = 0; index < m_timers.size(); index++) {
				Timer timerCache = m_timers[index];
				if (timerCache.update()) {
					
					TimerEvent te = TimerEvent(timerCache.getName(), timerCache.getFinishedTime());
					wm.onEvent(&te);
				}
			}
			return 0;
		}
		return -1;
	}

	Timer* TimerManager::getTimer(std::string timer_name) {
		if (this->isStarted()) {
			for (int search = 0; search < m_timers.size(); search++) {
				if (m_timers[search].getName().compare(timer_name) == 0) {
					return &m_timers[search];
				}
			}
		}
		return nullptr;

	}

	int TimerManager::addTimer(std::string timer_name) {
		if (this->isStarted()) {
			for (int check = 0; check < m_timers.size(); check++) {
				if (m_timers[check].getName().compare(timer_name) == 0) {
					return -1;
				}
			}

			m_timers.push_back(Timer(timer_name));
			return 0;
		}
		return -1;
	}

	int TimerManager::addTimer(std::string timer_name, unsigned long long timer_setting, bool looping) {
		if (this->isStarted()) {
			for (int check = 0; check < m_timers.size(); check++) {
				if (m_timers[check].getName().compare(timer_name) == 0) {
					return -1;
				}
			}

			m_timers.push_back(Timer(timer_name,timer_setting,looping));
			return 0;
		}
		return -1;
	}

	int TimerManager::removeTimer(std::string timer_name) {
		if (this->isStarted()) {
			int found = -1;
			for (int search = m_timers.size() - 1; search >= 0; search--) {
				if (m_timers[search].getName().compare(timer_name) == 0) {
					m_timers[search] = m_timers[m_timers.size() - 1];
					m_timers.pop_back();
					found = 0;
				}
			}
			return found;
		}
		return -1;
	}

	int TimerManager::getCount()const {
		if (this->isStarted()) {
			return m_timers.size();
		}
		return -1;
	}

	bool TimerManager::isEmpty()const {
		if (this->isStarted()) {
			return m_timers.empty();
		}
		return true;
		

	}

	int TimerManager::clear() {
		if (this->isStarted()) {
			m_timers.clear();
			return 0;
		}
		return -1;
	}
}