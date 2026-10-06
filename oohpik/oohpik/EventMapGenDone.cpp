#include "EventMapGenDone.h"

namespace ookpik {
	EventMapGenDone::EventMapGenDone() {
		this->setType("ookpik::mapGenDone");
		m_genTime = 0;
		m_map_objects = df::ObjectList();
		m_error = false;
		m_error_messages = std::vector<std::string>();
		m_error_messages.push_back("no error");
	}
	EventMapGenDone::EventMapGenDone(df::ObjectList mapObjects, unsigned long long genTimeMS) {
		m_genTime = genTimeMS;
		m_map_objects = mapObjects;
	}
	EventMapGenDone::EventMapGenDone(std::vector<std::string> error_message) {
		m_error = true;
		m_error_messages = error_message;
	}
	df::ObjectList EventMapGenDone::getMapObjects()const {
		return m_map_objects;
	}
	void EventMapGenDone::setMapObjects(df::ObjectList new_map_objects) {
		m_map_objects = new_map_objects;
	}
	unsigned long long EventMapGenDone::getGenTime()const {
		return m_genTime;
	}
	void EventMapGenDone::setGenTime(unsigned long long new_gen_time) {
		m_genTime = new_gen_time;
	}

	void EventMapGenDone::setError(bool new_error) {
		m_error = new_error;
	}
	bool EventMapGenDone::getError()const {
		return m_error;
	}
	void EventMapGenDone::setErrorMessage(std::vector<std::string> new_error_messages) {
		m_error_messages = new_error_messages;
	}
	std::vector<std::string> EventMapGenDone::getErrorMessage()const {
		return m_error_messages;
	}

}