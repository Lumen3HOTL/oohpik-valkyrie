#include "EventRegistrationKeeper.h"

namespace df {
	EventRegistrationKeeper::EventRegistrationKeeper() {
		m_eventID = "Undefined_Event";
		m_interested_parties = ObjectList();
	}
	EventRegistrationKeeper::EventRegistrationKeeper(std::string event_id_string) {
		m_eventID = event_id_string;
		m_interested_parties = ObjectList();
	}
	int EventRegistrationKeeper::registerInterest(Object* p_o) {
		return m_interested_parties.insert(p_o);
	}

	int EventRegistrationKeeper::unregisterInterest(Object* p_o) {
		return m_interested_parties.remove(p_o);
	}

	int EventRegistrationKeeper::sendEvent(const Event* p_e) {
		int count = 0;
		for (int receiver = 0; receiver < m_interested_parties.getCount(); receiver++) {
			count+=m_interested_parties[receiver]->eventHandler(p_e);
		}
		return count;
	}

	std::string EventRegistrationKeeper::getEventId()const {
		return m_eventID;
	}

	void EventRegistrationKeeper::setEventId(std::string new_event_id) {
		m_eventID = new_event_id;
	}

	void EventRegistrationKeeper::clearRegistration() {
		m_interested_parties.clear();
	}

	int EventRegistrationKeeper::getRegistrationCount()const {
		return m_interested_parties.getCount();
	}

	bool EventRegistrationKeeper::isEmpty()const {
		return m_interested_parties.isEmpty();

	}

	ObjectList EventRegistrationKeeper::getRegisteredObjects()const {
		ObjectList tempList;

		for (int index = 0; index < m_interested_parties.getCount(); index++) {
			tempList.insert(m_interested_parties[index]);
		}
		return tempList;
	}
}