#include "Object.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "ResourceManager.h"
#include <iostream>
#include "EventManager.h"
namespace df {

	Object::Object() {



		static unsigned long long nextObjectId = 0;
		m_id = nextObjectId;
		nextObjectId++;
		this->setType(UNDEFINED_OBJECT);
		m_position = Vector();
		m_visible = true;
		m_altitude = (unsigned int)(MAX_ALTITUDE / 2u);
		m_speed = 0;
		m_direction = Vector();
		m_solidness = SOFT;
		m_no_soft = false;
		WorldManager& worldKeeper = WorldManager::getInstance();
		worldKeeper.insertObject(this);
		m_animation = Animation();
		m_camera_affected = true;
	}



	Object::~Object() {
		EventManager& eventKeeper = EventManager::getInstance();
		eventKeeper.removeObjectFromAll(this);
		WorldManager& worldKeeper = WorldManager::getInstance();
		worldKeeper.removeObject(this);
		
	}

	void Object::setId(unsigned long long new_id) {
		m_id = new_id;
	}

	unsigned long long Object::getId() const {
		return m_id;
	}

	void Object::setType(std::string new_type) {
		m_type = new_type;
	}

	std::string Object::getType() const {
		return m_type;
	}


	void Object::setPosition(Vector new_pos) {
		m_position = new_pos;
	}

	Vector Object::getPosition() const {
		return m_position;
	}

	int Object::eventHandler(const Event* p_e) {
		return 0;
	}

	void Object::setVisible(const bool newVisiblity) {
		m_visible = newVisiblity;
	}

	bool Object::getVisible() const {
		return m_visible;
	}
	int Object::draw() {
		if (m_visible) {
			if (m_animation.draw(m_position) == -1) {
				std::string defaultTexture = "<UO>";
				DisplayManager& dman = DisplayManager::getInstance();


				return dman.drawString(this->getPosition(), defaultTexture, CENTER_JUSTIFIED, UNDEFINED_COLOR);
			}
			else {
				return 0;
			}


		}
		return 0;

	}

	int Object::setAltitude(unsigned int new_altitude) {
		if ((new_altitude <= MAX_ALTITUDE) && (new_altitude >= 0)) {
			m_altitude = new_altitude;
			return 0;
		}
		return -1;
	}

	int Object::getAltitude() const {
		return m_altitude;
	}


	// Set speed of Object.
	void Object::setSpeed(float speed) {
		m_speed = speed;
	}

	// Get speed of Object.
	float Object::getSpeed() const {
		return m_speed;
	}

	// Set direction of Object.
	void Object::setDirection(Vector new_direction) {
		m_direction = new_direction;
		m_direction.normalize();
	}

	// Get direction of Object.
	Vector Object::getDirection() const {
		return m_direction;
	}

	// Set direction and speed of Object.
	void Object::setVelocity(Vector new_velocity) {
		m_direction = Vector(new_velocity);
		m_direction.normalize();

		m_speed = new_velocity.getMagnitude();
	}

	// Get velocity of Object based on direction and speed.
	Vector Object::getVelocity() const {
		Vector temp = Vector(m_direction);
		temp.scale(m_speed);
		return temp;
	}

	// Predict Object position based on speed and direction.
	// Return predicted position.
	Vector Object::predictPosition() {
		return Vector(m_position) + this->getVelocity();

	}



	bool Object::isSolid() const {
		// True if HARD or SOFT, else false.
		switch (m_solidness) {
		case HARD:
			return true;
		case SOFT:
			return true;
		default:
			return false;
		}
	}

	// Set object solidness, with checks for consistency.
	// Return 0 if ok, else -1.
	int Object::setSolidness(Solidness new_solid) {
		if ((new_solid >= HARD) && (new_solid <= SPECTRAL)) {
			m_solidness = new_solid;
			return 0;
		}

		return -1;
	}

	// Return object solidness.
	Solidness Object::getSolidness() const {
		return m_solidness;
	}


	// Set 'no_soft' setting (true - cannot move on to SOFT Objects).
	void Object::setNoSoft(bool new_no_soft) {
		m_no_soft = new_no_soft;
	}

	// Get 'no_soft' setting (true - cannot move on to SOFT Objects).
	bool Object::getNoSoft() const {
		return m_no_soft;
	}


	// Set Sprite for this Object to animate.
		// Return 0 if ok, else -1.
	int Object::setSprite(std::string sprite_label) {
		ResourceManager& rm = ResourceManager::getInstance();
		Sprite* tempSprite = nullptr;
		Animation tempAnim = Animation();
		tempSprite = rm.getSprite(sprite_label);
		if (tempSprite == nullptr) {
			return -1;
		}
		tempAnim.setSprite(tempSprite);
		m_animation = tempAnim;
		m_box = m_animation.getBox();
		return 0;
	}

	// Set Animation for this Object to new one.
	// Set bounding box to size of associated Sprite.
	void Object::setAnimation(Animation new_animation) {
		m_animation = new_animation;
		//not doen yet, still needs box implementation
		m_box = m_animation.getBox();
	}

	// Get Animation for this Object.
	Animation Object::getAnimation() const {
		return m_animation;
	}


	// Set Object's bounding box.
	void Object::setBox(Box new_box) {
		m_box = new_box;
	}

	// Get Object's bounding box.
	Box Object::getBox() const {
		return m_box;
	}


	bool Object::getCameraAffected() const {
		return m_camera_affected;
	}

	void Object::setCameraAffected(bool new_Camera_Lock_state) {
		m_camera_affected = new_Camera_Lock_state;
	}
}