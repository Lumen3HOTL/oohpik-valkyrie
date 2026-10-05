#pragma once


#include "Vector.h"

namespace ookpik {
	class VectorQueueEntry {
		private:
			bool m_empty;
			int	m_lastEntryIndex;
			int m_nextEntryIndex;
			df::Vector m_data;
	public:
		VectorQueueEntry();
		bool getEmpty()const;
		void setEmpty(bool new_empty);
		void reset();
		void setLastEntryIndex(int entry);
		void setNextEntryIndex(int entry);
		int getLastEntryIndex()const;
		int getNextEntryIndex()const;
		df::Vector getData()const;
		void setData(df::Vector new_data);
	};
}