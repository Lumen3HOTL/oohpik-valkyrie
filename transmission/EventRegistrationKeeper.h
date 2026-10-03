#pragma once
#include "event.h"
#include <string>

#include "Object.h"
#include "ObjectList.h"
namespace df {
	class EventRegistrationKeeper {
	private:
		std::string m_eventID;
		ObjectList m_interested_parties;

	public:
		EventRegistrationKeeper();
		EventRegistrationKeeper(std::string event_id_string);

		int registerInterest(Object* p_o);

		int unregisterInterest(Object* p_o);

		int sendEvent(const Event* p_e);

		std::string getEventId()const;
		
		void setEventId(std::string new_event_id);

		void clearRegistration();

		int getRegistrationCount()const;

		bool isEmpty()const;

		ObjectList getRegisteredObjects()const;

	};
}