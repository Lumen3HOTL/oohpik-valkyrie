#include "VectorQueueEntry.h"

namespace ookpik {
	VectorQueueEntry::VectorQueueEntry() {
		m_empty = true;
		m_data = df::Vector();
		m_lastEntryIndex = -1;
		m_nextEntryIndex = -1;
	}
	bool VectorQueueEntry::getEmpty()const {
		return m_empty;
	}
	void VectorQueueEntry::setEmpty(bool new_empty) {
		m_empty = new_empty;
	}
	void VectorQueueEntry::reset() {
		m_empty = true;
		m_data = df::Vector();
		m_lastEntryIndex = -1;
		m_nextEntryIndex = -1;
	}
	void VectorQueueEntry::setLastEntryIndex(int new_last_entry) {
		m_lastEntryIndex = new_last_entry;
	}
	void VectorQueueEntry::setNextEntryIndex(int new_next_entry) {
		m_nextEntryIndex = new_next_entry;
	}
	int VectorQueueEntry::getLastEntryIndex()const {
		return m_lastEntryIndex;
	}
	int VectorQueueEntry::getNextEntryIndex()const {
		return m_nextEntryIndex;
	}
	df::Vector VectorQueueEntry::getData()const {
		return m_data;
	}
	void VectorQueueEntry::setData(df::Vector new_data) {
		m_data=new_data;
	}
}