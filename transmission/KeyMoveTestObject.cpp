#include "KeyMoveTestObject.h"
#include "EventOut.h"
#include "WorldManager.h"
namespace test {
	KeyMoveTestObject::KeyMoveTestObject() {
		this->setType("KeyMoveTestObject");
		m_downKey = false;
		m_upKey = false;
		m_leftKey = false;
		m_rightKey = false;
		m_zup = false;
		m_zdown = false;
		m_moveTimer = 0;
		m_moveTimerMax = 4;
		m_out = false;
		m_out_decay = 0;
		m_out_decay_set = 15;
		this->setAltitude(0);
	}

	KeyMoveTestObject::~KeyMoveTestObject() {

	}

	int KeyMoveTestObject::draw() {
		df::DisplayManager& dm = df::DisplayManager::getInstance();
		if (this->getVisible()) {
			if (m_out) {
				return dm.drawString(this->getPosition(), "<keyMoveTest>\n<out!>\n<keyMoveTest>", df::CENTER_JUSTIFIED, df::RED, df::CENTER_ALLIGNED);
			}
			else {
				return dm.drawString(this->getPosition(), "<keyMoveTest>\n<in>\n<keyMoveTest>", df::CENTER_JUSTIFIED, df::YELLOW, df::CENTER_ALLIGNED);
			}
		}
		
		
		return 0;
	}

	



	int KeyMoveTestObject::eventHandler(const df::Event* p_e) {
		if (p_e->getType().compare(df::STEP_EVENT) == 0) {
			if (m_out) {
				m_out_decay++;
				if (m_out_decay > m_out_decay_set) {
					m_out_decay = 0;
					m_out = false;
				}
			}
			m_moveTimer++;
			if (m_moveTimer > m_moveTimerMax) {
				df::WorldManager& wm = df::WorldManager::getInstance();
				m_moveTimer = 0;
				if (m_upKey) {
					wm.moveObject(this,df::Vector(this->getPosition().getX(), this->getPosition().getY()-2));
				}
				else if (m_downKey) {
					wm.moveObject(this, df::Vector(this->getPosition().getX(), this->getPosition().getY() + 2));
				}
				if (m_leftKey) {
					wm.moveObject(this, df::Vector(this->getPosition().getX() - 1, this->getPosition().getY()));
				}
				else if (m_rightKey) {
					wm.moveObject(this, df::Vector(this->getPosition().getX() + 1, this->getPosition().getY()));
				}
				if (m_zup) {
					if (this->getAltitude() < df::MAX_ALTITUDE) {
						this->setAltitude(this->getAltitude() + 1);
					}
				}
				else if (m_zdown) {
					if (this->getAltitude() > 0) {
						this->setAltitude(this->getAltitude() - 1);
					}
				}
			}
		}
		else if (p_e->getType().compare(df::KEYBOARD_EVENT) == 0) {

			//cast it to the correct event type
			const df::EventKeyboard* keyEvent = dynamic_cast<const df::EventKeyboard*> (p_e);
			if (keyEvent->getKeyboardAction() == df::KEY_RELEASED) {
				if (keyEvent->getKey() == df::Keyboard::A) {
					m_leftKey = false;
				}
				if (keyEvent->getKey() == df::Keyboard::D) {
					m_rightKey = false;
				}
				if (keyEvent->getKey() == df::Keyboard::W) {
					m_upKey = false;
				}
				if (keyEvent->getKey() == df::Keyboard::S) {
					m_downKey = false;
				}
				if (keyEvent->getKey() == df::Keyboard::Q) {
					m_zdown = false;
				}
				if (keyEvent->getKey() == df::Keyboard::E) {
					m_zup = false;
				}
			}else if (keyEvent->getKeyboardAction() == df::KEY_PRESSED) {
				if (keyEvent->getKey() == df::Keyboard::A) {
					m_leftKey = true;
				}
				else if (keyEvent->getKey() == df::Keyboard::D) {
					m_rightKey = true;
				}
				if (keyEvent->getKey() == df::Keyboard::W) {
					m_upKey = true;
				}
				else if (keyEvent->getKey() == df::Keyboard::S) {
					m_downKey = true;
				}
				if (keyEvent->getKey() == df::Keyboard::Q) {
					m_zdown = true;
				}
				else if (keyEvent->getKey() == df::Keyboard::E) {
					m_zup = true;
				}
			}
		}
		else if (p_e->getType().compare(df::OUT_EVENT)==0) {
			m_out = true;
			m_out_decay = 0;
		}

		return 0;
	}
}