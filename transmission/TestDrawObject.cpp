#include "TestDrawObject.h"
#include "Color.h"
#include "EventStep.h"
#include "DisplayManager.h"
namespace test {
	TestDrawObject::TestDrawObject() {
		m_frameCounter = 0;
		m_color = df::UNDEFINED_COLOR;
		m_sprite = "++drawTest++";
		this->setType("TestDrawObject");
	}

	TestDrawObject::~TestDrawObject() {

	}

	int TestDrawObject::draw() {
		if (this->getVisible()) {
			df::DisplayManager& display = df::DisplayManager::getInstance();

			return display.drawString(this->getPosition(), m_sprite, df::CENTER_JUSTIFIED, m_color);
		}
	}

	int TestDrawObject::setColor(df::Color newcolor) {
		m_color = newcolor;
		return 0;
	}

	int TestDrawObject::setSprite(std::string newTexture) {
		m_sprite = newTexture;
		return 0;
	}

	
}