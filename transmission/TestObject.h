#pragma once
#include "Object.h"
#include "Event.h"
#include "ObjectList.h"
#include "Clock.h"
namespace test {
	class TestObject :public  df::Object {
	private:
		unsigned int m_frameCounter;
		bool m_firstReceivedStep;
		df::ObjectList m_testObjects;
		int m_dmode;
		int m_count2;
		int m_count1;
		int m_count0;
		int m_dir;
		int m_circulator;
		df::Clock m_timer;
		unsigned int m_runtime;
		df::ObjectList m_testAnimObjects;

	public:
		TestObject();

		~TestObject();

		int draw();

		int eventHandler(const df::Event* p_e);
	};
}
