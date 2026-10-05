#include "VectorQueue.h"


namespace ookpik {
	VectorQueue::VectorQueue() {
		m_elements = std::vector<VectorQueueEntry>();
		m_size = 0;
		m_frontIndex = -1;
		m_backIndex = -1;
		m_last_freed = -1;
		m_first_freed = -1;
	}
	void VectorQueue::push(df::Vector add) {
		if (m_size <= 0) {

		}
		else if (m_first_freed != -1) {
			
		}

		m_size++;
	}
	df::Vector VectorQueue::pop() {
		if (m_size <= 0) {
			return df::Vector();
		}
		
		m_size--;
		if (m_size <= 0) {
			this->clear();
		}
	}
	df::Vector VectorQueue::peek(){
		if (m_size <= 0) {
			return df::Vector();
		}
		return m_elements.at(m_frontIndex).getData();
	}

	void VectorQueue::clear() {
		m_elements.clear();
		m_size = 0;
		m_frontIndex = -1;
		m_backIndex = -1;
		m_last_freed = -1;
		m_first_freed = -1;
	}

	int VectorQueue::getLength()const {
		return m_size;
	}

	
}