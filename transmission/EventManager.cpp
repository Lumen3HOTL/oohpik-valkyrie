#include "EventManager.h"
#include <unordered_set>
namespace df {
	EventManager::EventManager() {
		this->setType("EventManager");
		m_eventRegistrationKeepers = std::deque<EventRegistrationKeeper>();
	}
	
	EventManager& EventManager::getInstance() {
		static EventManager em = EventManager();
		return em;
	}


	int EventManager::startUp() {
		if (this->isStarted()) {
			return -1;
		}
		m_eventRegistrationKeepers.clear();
		return Manager::startUp();

	}


	void EventManager::shutDown() {
		m_eventRegistrationKeepers.clear();
		Manager::shutDown();
	}


	int EventManager::addEventType(std::string new_event_type) {
		if (this->isStarted()) {
			for (int search = 0; search < m_eventRegistrationKeepers.size(); search++) {
				if (m_eventRegistrationKeepers[search].getEventId().compare(new_event_type) == 0) {
					return -1;
				}
			}

			m_eventRegistrationKeepers.push_back(EventRegistrationKeeper(new_event_type));
			return 0;
		}
		return -1;
		
	}

	int EventManager::removeEventType(std::string event_type_to_delete) {
		if (this->isStarted()) {
			int found = -1;
			for (int search = m_eventRegistrationKeepers.size() - 1; search >= 0; search--) {
				if (m_eventRegistrationKeepers[search].getEventId().compare(event_type_to_delete) == 0) {
					m_eventRegistrationKeepers[search] = m_eventRegistrationKeepers[m_eventRegistrationKeepers.size() - 1];
					m_eventRegistrationKeepers.pop_back();
					found = 0;
				}
			}
			return found;
		}
		return -1;
	}

	int EventManager::clearEvents() {
		if (this->isStarted()) {
			m_eventRegistrationKeepers.clear();
			return 0;
		}
		return -1;
	}


	int EventManager::registerEvent(Object* p_o, std::string event) {
		if (this->isStarted()) {
			for (int type = 0; type < m_eventRegistrationKeepers.size(); type++) {
				if (m_eventRegistrationKeepers[type].getEventId().compare(event) == 0) {
					return m_eventRegistrationKeepers[type].registerInterest(p_o);
					
				}
			}
			if (this->addEventType(event) != 0) {
				return -1;
			}
			return m_eventRegistrationKeepers[m_eventRegistrationKeepers.size() - 1].registerInterest(p_o);
			
		}
		return -1;
	}

	int EventManager::unregisterEvent(Object* p_o, std::string event) {
		if (this->isStarted()) {
			for (int type = 0; type < m_eventRegistrationKeepers.size(); type++) {
				if (m_eventRegistrationKeepers[type].getEventId().compare(event) == 0) {
					return m_eventRegistrationKeepers[type].unregisterInterest(p_o);
				}
			}
		}
		return -1;
	}

	bool EventManager::hasRegisteredEvents()const {
		if (this->isStarted()) {
			return !m_eventRegistrationKeepers.empty();
		}
		return -1;
	}

	ObjectList EventManager::getRegisteredObjectsList()const {
		if (this->isStarted()) {
			std::unordered_set<Object*> found;
			ObjectList present;
			for (int event = 0; event < m_eventRegistrationKeepers.size(); event++) {
				ObjectList listCache = m_eventRegistrationKeepers[event].getRegisteredObjects();
				
				if (!listCache.isEmpty()) {
					for (int object = 0; object < listCache.getCount();object++) {
						if (!found.contains(listCache[object])) {
							found.insert(listCache[object]);
							present.insert(listCache[object]);
						}
					}
				}
				
			}
			return present;
		}
		return ObjectList();
	}



	bool EventManager::hasregisteredObjects()const {
		if (this->isStarted()) {
			return !this->getRegisteredObjectsList().isEmpty();
		}
		return false;
	}

	int EventManager::getRegisteredEventCount()const {
		if (this->isStarted()) {
			return m_eventRegistrationKeepers.size();
		}
		return -1;
	}

	int EventManager::getregistedObjectsCount()const {
		if (this->isStarted()) {
			return this->getRegisteredObjectsList().getCount();
		}
		return -1;
	}

	EventRegistrationKeeper* EventManager::getEventRegistrationKeeperForEventType(std::string event_type_string) {
		if (this->isStarted()) {
			for (int eventKeeper = 0; eventKeeper < m_eventRegistrationKeepers.size(); eventKeeper++) {
				if (m_eventRegistrationKeepers[eventKeeper].getEventId().compare(event_type_string) == 0) {
					return &m_eventRegistrationKeepers[eventKeeper];
				}
			}
		}
		return nullptr;
	}

	std::vector<std::string> EventManager::getRegisteredEventTypes()const {
		if (this->isStarted()) {
			if (!m_eventRegistrationKeepers.empty()) {
				std::vector<std::string> templist;
				for (int registrar = 0; registrar < m_eventRegistrationKeepers.size(); registrar++) {
					templist.push_back(m_eventRegistrationKeepers[registrar].getEventId());
				}
				return templist;
			}
			
			
		}
		return std::vector<std::string>();
	}


	int EventManager::removeObjectFromAll(Object* p_o) {
		if (this->isStarted()) {
			int found = 0;
			
			for (int registrar = 0; registrar < m_eventRegistrationKeepers.size(); registrar++) {
				if (m_eventRegistrationKeepers[registrar].unregisterInterest(p_o) == 0) {
					found++;
				}
			}
			return found;
		}
		return -1;
		
	}

	int EventManager::sendEvent(const Event* p_e) {
		if (this->isStarted()) {
			for (int type = 0; type < m_eventRegistrationKeepers.size(); type++) {
				if (m_eventRegistrationKeepers[type].getEventId().compare(p_e->getType()) == 0) {
					return m_eventRegistrationKeepers[type].sendEvent(p_e);
				}
			}
		}
		return -1;
	}
}