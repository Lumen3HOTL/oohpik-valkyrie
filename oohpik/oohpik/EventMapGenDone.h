#pragma once
#include "Event.h"
#include "ObjectList.h"
#include <string>
namespace ookpik {
	class EventMapGenDone :public df::Event {
	private:
		df::ObjectList m_map_objects;
		unsigned long long m_genTime;
		unsigned long long m_buildTime;
		bool m_error;
		std::vector<std::string> m_error_messages;
	public:
		EventMapGenDone();
		EventMapGenDone(df::ObjectList mapObjects, unsigned long long genTimeMS, unsigned long long buildTimeMS);
		EventMapGenDone(std::vector<std::string> errror_messages);
		df::ObjectList getMapObjects()const;
		void setMapObjects(df::ObjectList new_map_objects);
		unsigned long long getGenTime()const;
		void setGenTime(unsigned long long new_gen_time);
		unsigned long long getBuildTime()const;
		void setBuildTime(unsigned long long new_gen_time);
		void setError(bool new_error);
		bool getError()const;
		void setErrorMessage(std::vector<std::string> new_error_messages);
		std::vector<std::string> getErrorMessage()const;
	};
}