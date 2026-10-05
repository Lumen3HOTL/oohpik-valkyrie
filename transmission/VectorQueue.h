#pragma once
#include <vector>
#include "Vector.h"
#include "VectorQueueEntry.h"
namespace ookpik {
	class VectorQueue{
		private:
			std::vector<VectorQueueEntry> m_elements;
			int m_frontIndex;
			int m_backIndex;
			int m_size;
			int m_last_freed;
			int m_first_freed;
			
		public:
			VectorQueue();
			void push(df::Vector add);
			df::Vector pop();
			df::Vector peek();
			void clear();
			int getLength()const;
			void compact();
		};

	
}