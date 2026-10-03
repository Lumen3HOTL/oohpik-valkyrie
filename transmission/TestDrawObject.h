#pragma once
#include "Object.h"
#include "Event.h"
#include "Color.h"
#include "EventStep.h"
namespace test {
	class TestDrawObject :public  df::Object {
	private:
		unsigned int m_frameCounter;
		std::string m_sprite;
		df::Color m_color;

	public:
		TestDrawObject();

		~TestDrawObject();

		int draw();

		int setColor(df::Color newcolor);

		int setSprite(std::string newTexture);

		
	};
}