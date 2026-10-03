#pragma once
#include "Object.h"
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "color.h"
#include "EventStep.h"
#include "WorldManager.h"
namespace test {
	class KeyMoveTestObject :public  df::Object {
	private:
		unsigned int m_moveTimerMax;
		unsigned int m_moveTimer;
		bool m_upKey;
		bool m_downKey;
		bool m_leftKey;
		bool m_rightKey;
		bool m_zup;
		bool m_zdown;
		bool m_out;
		unsigned int m_out_decay;
		unsigned int m_out_decay_set;

		
	public:
		KeyMoveTestObject();

		~KeyMoveTestObject();

		int draw();

		



		int eventHandler(const df::Event* p_e);
	};
}