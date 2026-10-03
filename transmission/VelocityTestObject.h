#pragma once
#include "object.h"
#include "Color.h"
#include "DisplayManager.h"
#include "Event.h"
#include "EventCollision.h"
#include "EventOut.h"
#include "EventStep.h"
#include <string>
#include "LogManager.h"
namespace test {
	class VelocityTestObject : public  df::Object {
	private:
		std::string m_sprite;
		bool m_collison;
		std::string m_colided;
		unsigned int m_message_timeout;
		unsigned int m_collision_message_timer;
		bool m_out;
		unsigned int m_out_message_timer;
		df::Color m_color;
		unsigned int m_frame_count;
		unsigned int m_log_limiter;
		unsigned int m_log_timer;
		int generateSprite();
		df::Vector m_collsionPos;
	public:

		int resetLogTimer();

		VelocityTestObject();

		~VelocityTestObject();

		int draw();

		int eventHandler(const df::Event* p_e);

		int setSprite(std::string newTexture);
	};
}