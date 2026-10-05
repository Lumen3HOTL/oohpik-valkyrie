#include "EventMapGenDone.h"

namespace ookpik {
	EventMapGenDone::EventMapGenDone() {
		this->setType("ookpik::mapGenDone");
		m_genTime = 0;
		m_map_objects = df::ObjectList();
	}
	EventMapGenDone::EventMapGenDone(df::ObjectList mapObjects, unsigned long long genTimeMS) {
		m_genTime = genTimeMS;
		m_map_objects = mapObjects;
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
}