#pragma once
#include "Event.h"
#include "EventRegistrationKeeper.h"
#include "Manager.h"

namespace df {
	class EventManager : public Manager {
		EventManager(); // P r i v a t e ( a s i n g l e t o n ) .
		EventManager(EventManager const&); // Don ’ t a l l o w copy .
		void operator =(EventManager const&); // Don ’ t a l l o w a s s i g n m e n t .
		std::vector<EventRegistrationKeeper> m_eventRegistrationKeepers;
		
	public:
		static EventManager& getInstance();


		int startUp();

		
		void shutDown();
		

		int addEventType(std::string new_event_type);

		int removeEventType(std::string new_event_type);

		int clearEvents();

		int registerEvent(Object* p_o, std::string event);

		int unregisterEvent(Object* p_o, std::string event);

		bool hasRegisteredEvents()const;

		bool hasregisteredObjects()const;

		int getRegisteredEventCount()const;

		int getregistedObjectsCount()const;

		EventRegistrationKeeper* getEventRegistrationKeeperForEventType(std::string event_type_string);

		std::vector<std::string> getRegisteredEventTypes()const;

		ObjectList getRegisteredObjectsList()const;

		int removeObjectFromAll(Object* p_o);

		int sendEvent(const Event* p_e);

	};
}