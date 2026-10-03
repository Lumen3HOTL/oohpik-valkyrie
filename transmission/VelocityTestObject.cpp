#include "VelocityTestObject.h"

namespace test {

	int VelocityTestObject::generateSprite() {
		std::string base0 = std::string("<VelocityTestObject #").append(std::to_string(this->getId())).append(">\n");
		std::string base1;
		std::string base2;
		std::string base3;
		//create a hardness string
		std::string hardness = "";
		switch (this->getSolidness()) {
		case df::HARD:
			hardness = "HARD";
			break;
		case df::SOFT:
			hardness = "SOFT";
			break;
		default:
			hardness = "SPECTRAL";
			break;
		}
		int width = 0;
		//the massive append stack that forms the the texture in real time. relitively simple, just a lot of string joins
		base1=std::string("<pos: x:").append(std::to_string(this->getPosition().getX())).append(" y:").append(std::to_string(this->getPosition().getY())).append(">\n");
		base2=std::string("< move vector: x:").append(std::to_string(this->getDirection().getX())).append(" y:").append(std::to_string(this->getDirection().getY())).append(" speed:").append(std::to_string(this->getSpeed())).append(" dir:").append(std::to_string(this->getDirection().getDegreeDirection())).append(" >\n");
		base3=std::string("< current state: colided with: ").append(m_colided).append(" out state: ").append(std::to_string(m_out)).append(" solidness: ").append(hardness).append(">");
		width = base0.length();
		if (width < base1.length()) {
			width = base1.length();
		}
		else if (width < base2.length()) {
			width = base2.length();
		}
		else if (width < base3.length()) {
			width = base3.length();
		}

		m_collsionPos = df::Vector(width / 2, -2);
		this->setBox(df::Box(m_collsionPos, width, 4));

		this->setSprite(base0.append(base1).append(base2).append(base3));
		if (m_collison&&m_out) {
			m_color = df::YELLOW;
		}
		else if (m_collison) {
			m_color = df::MAGENTA;
		}
		else if(m_out){
			m_color = df::CYAN;
		}
		else {
			m_color = df::WHITE;
		}
		return 0;
	}

	VelocityTestObject::VelocityTestObject() {
		this->setType("VelocityTestObject");
		this->setPosition(df::Vector(40, 12));
		m_colided = "none";
		m_collison = false;
		m_out = false;
		m_message_timeout = 30;
		m_collision_message_timer = 0;
		m_out_message_timer = 0;
		m_frame_count = 0;
		this->generateSprite();
		m_log_limiter = 15;
		m_log_timer = 0;
		m_collsionPos = df::Vector(0, -2);
		this->setBox(df::Box(m_collsionPos, 0, 4));
		this->setSolidness(df::HARD);
	}

	VelocityTestObject::~VelocityTestObject() {

	}

	int VelocityTestObject::draw() {
		if (this->getVisible()) {
			return df::DisplayManager::getInstance().drawString(this->getPosition(), m_sprite, df::CENTER_JUSTIFIED, m_color, df::CENTER_ALLIGNED);
		}
		return 0;
	}

	int VelocityTestObject::eventHandler(const df::Event* p_e) {

		if (p_e->getType().compare(df::STEP_EVENT) == 0) {
			if (m_log_limiter > m_log_timer) {
				//create a hardness string
				std::string hardness = "";
				switch (this->getSolidness()) {
				case df::HARD:
					hardness = "HARD";
					break;
				case df::SOFT:
					hardness = "SOFT";
					break;
				default:
					hardness = "SPECTRAL";
					break;
				}
				//the massive append stack that creates the log message
				std::string base = std::string("(VelocityTestObject #").append(std::to_string(this->getId())).append(" frame: ").append(std::to_string(m_frame_count));
				base.append(" pos: x:").append(std::to_string(this->getPosition().getX())).append(" y:").append(std::to_string(this->getPosition().getY()));
				base.append(" move vector: x:").append(std::to_string(this->getDirection().getX())).append(" y:").append(std::to_string(this->getDirection().getY())).append(" speed:").append(std::to_string(this->getSpeed())).append(" dir:").append(std::to_string(this->getDirection().getDegreeDirection()));
				base.append(" current state:").append(" solidness: ").append(hardness).append(")");
				df::LogManager::getInstance().writeLog(base.c_str());
				
				m_log_timer++;
			}
			this->generateSprite();
			//the timer logic to turn off the messages after a second
			if (m_collison) {
				m_collision_message_timer++;
				if (m_collision_message_timer > m_message_timeout) {
					m_collison = false;
					m_collision_message_timer = 0;
					m_colided = "none";
				}
			}
			if (m_out) {
				m_out_message_timer++;
				if (m_out_message_timer > m_message_timeout) {
					m_out = false;
					m_out_message_timer = 0;
				}
			}
			m_frame_count++;
			this->setVisible(true);
			return 1;
		}
		else if (p_e->getType().compare(df::OUT_EVENT) == 0) {
			//turn on and reset the timer for the out message
			m_out = true;
			m_out_message_timer = 0;
			this->setVisible(false);
			//create a hardness string
			std::string hardness = "";
			switch (this->getSolidness()) {
			case df::HARD:
				hardness = "HARD";
				break;
			case df::SOFT:
				hardness = "SOFT";
				break;
			default:
				hardness = "SPECTRAL";
				break;
			}
			// the massive append stack that creates the log message
			std::string base = std::string("(VelocityTestObject #").append(std::to_string(this->getId())).append(" frame: ").append(std::to_string(m_frame_count));
			base=base.append(" pos: x:").append(std::to_string(this->getPosition().getX())).append(" y:").append(std::to_string(this->getPosition().getY())).append(" solidness: ").append(hardness);
			
			base=base.append(" current state:").append(" out state: ").append(std::to_string(m_out)).append(" solidness: ").append(hardness).append(")");
			df::LogManager::getInstance().writeLog(base.c_str());
			return 1;
		}
		else if (p_e->getType().compare(df::COLLISION_EVENT) == 0) {
			//turn on and reset the timer for the collison message
			m_collison = true;
			m_collision_message_timer = 0;
			const df::EventCollision* collisionEvent = dynamic_cast<const df::EventCollision*> (p_e);
			
			
			
			std::string base = std::string("(VelocityTestObject #").append(std::to_string(this->getId())).append(" frame: ").append(std::to_string(m_frame_count));
			base=base.append(" pos: x:").append(std::to_string(this->getPosition().getX())).append(" y:").append(std::to_string(this->getPosition().getY()));
				

				

			//if the first of the colliding objects is this object, extract the important information form it, and assemble to colided string and set it
			if ((collisionEvent->getObject1()->getType().compare(this->getType()) == 0) && (collisionEvent->getObject1()->getId() == this->getId())) {
				Object* objectCache = collisionEvent->getObject2();
				df::Vector position=objectCache->getPosition();
				float direction=objectCache->getDirection().getDegreeDirection();
				float speed= objectCache->getSpeed();
				std::string hardness = "";
				switch (objectCache->getSolidness()) {
				case df::HARD:
					hardness = "HARD";
					break;
				case df::SOFT:
					hardness = "SOFT";
					break;
				default:
					hardness = "SPECTRAL";
					break;
				}
				m_colided = std::string("(type: ").append(objectCache->getType()).append(" id: ").append(std::to_string(objectCache->getId())).append(" x: ").append(std::to_string(position.getX())).append(" y: ").append(std::to_string(position.getY())).append(" dir: ").append(std::to_string(direction)).append(" speed: ").append(std::to_string(speed)).append(" solidness: ").append(hardness).append(")");
			}
			//if the second of the colliding objects is this object, extract the important information form it, and assemble to colided string and set it
			else if ((collisionEvent->getObject2()->getType().compare(this->getType()) == 0) && (collisionEvent->getObject2()->getId() == this->getId())) {
				Object* objectCache = collisionEvent->getObject1();
				df::Vector position = objectCache->getPosition();
				float direction = objectCache->getDirection().getDegreeDirection();
				float speed = objectCache->getSpeed();
				std::string hardness = "";
				switch (objectCache->getSolidness()) {
				case df::HARD:
					hardness = "HARD";
					break;
				case df::SOFT:
					hardness = "SOFT";
					break;
				default:
					hardness = "SPECTRAL";
					break;
				}
				m_colided = std::string("(type: ").append(objectCache->getType()).append(" id: ").append(std::to_string(objectCache->getId())).append(" x: ").append(std::to_string(position.getX())).append(" y: ").append(std::to_string(position.getY())).append(" dir: ").append(std::to_string(direction)).append(" speed: ").append(std::to_string(speed)).append(" solidness: ").append(hardness).append(")");
			}
			std::string hardness = "";
			switch (this->getSolidness()) {
			case df::HARD:
				hardness = "HARD";
				break;
			case df::SOFT:
				hardness = "SOFT";
				break;
			default:
				hardness = "SPECTRAL";
				break;
			}
			base=base.append(" current state: ").append(" collision: ").append(m_colided).append(" solidness: ").append(hardness).append(")");
			df::LogManager::getInstance().writeLog(base.c_str());
			return 1;
		}

		return 0;
	}

	int VelocityTestObject::resetLogTimer() {
		m_log_timer = 0;
		return 0;
	}

	int VelocityTestObject::setSprite(std::string newTexture) {
		m_sprite = newTexture;
		return 0;
	}
}