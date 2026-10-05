#pragma once
#include "Event.h"
#include "ObjectList.h"
namespace ookpik {
	class EventMapGenDone :public df::Event {
	private:
		df::ObjectList m_map_objects;
		unsigned long long m_genTime;
	public:
		EventMapGenDone();
		EventMapGenDone(df::ObjectList mapObjects, unsigned long long genTimeMS);
		df::ObjectList getMapObjects()const;
		void setMapObjects(df::ObjectList new_map_objects);
		unsigned long long getGenTime()const;
		void setGenTime(unsigned long long new_gen_time);
	};
}