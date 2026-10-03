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


	// S e t s p e e d o f O b j e c t .
	void Object::setSpeed(float speed) {
		m_speed = speed;
	}

	// Get s p e e d o f O b j e c t .
	float Object::getSpeed() const {
		return m_speed;
	}

	// S e t d i r e c t i o n o f O b j e c t .
	void Object::setDirection(Vector new_direction) {
		m_direction = new_direction;
		m_direction.normalize();
	}

	// Get d i r e c t i o n o f O b j e c t .
	Vector Object::getDirection() const {
		return m_direction;
	}

	// S e t d i r e c t i o n and s p e e d o f O b j e c t .
	void Object::setVelocity(Vector new_velocity) {
		m_direction = Vector(new_velocity);
		m_direction.normalize();

		m_speed = new_velocity.getMagnitude();
	}

	// Get v e l o c i t y o f O b j e c t b a s e d on d i r e c t i o n and s p e e d .
	Vector Object::getVelocity() const {
		Vector temp = Vector(m_direction);
		temp.scale(m_speed);
		return temp;
	}

	// P r e d i c t O b j e c t p o s i t i o n b a s e d on s p e e d and d i r e c t i o n .
	// Return p r e d i c t e d p o s i t i o n .
	Vector Object::predictPosition() {
		return Vector(m_position) + this->getVelocity();

	}



	bool Object::isSolid() const {
		// True i f HARD o r SOFT, e l s e f a l s e .
		switch (m_solidness) {
		case HARD:
			return true;
		case SOFT:
			return true;
		default:
			return false;
		}
	}

	// S e t o b j e c t s o l i d n e s s , w i t h c h e c k s f o r c o n s i s t e n c y .
	// Return 0 i f ok , e l s e −1.
	int Object::setSolidness(Solidness new_solid) {
		if ((new_solid >= HARD) && (new_solid <= SPECTRAL)) {
			m_solidness = new_solid;
			return 0;
		}

		return -1;
	}

	// Return o b j e c t s o l i d n e s s .
	Solidness Object::getSolidness() const {
		return m_solidness;
	}


	// S e t ‘ no s o f t ’ s e t t i n g ( t r u e − c a n n o t move o n to SOFT O b j e c t s ) .
	void Object::setNoSoft(bool new_no_soft) {
		m_no_soft = new_no_soft;
	}

	// Get ‘ no s o f t ’ s e t t i n g ( t r u e − c a n n o t move o n to SOFT O b j e c t s ) .
	bool Object::getNoSoft() const {
		return m_no_soft;
	}


	// S e t S p r i t e f o r t h i s O b j e c t t o a n i m a te .
		// Return 0 i f ok , e l s e −1.
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
		return 0;
	}

	// S e t Animation f o r t h i s O b j e c t t o new one .
	// S e t b o u n d i n g b o x t o s i z e o f a s s o c i a t e d S p r i t e .
	void Object::setAnimation(Animation new_animation) {
		m_animation = new_animation;
		//not doen yet, still needs box implementation
		m_box = m_animation.getBox();
	}

	// Get Animation f o r t h i s O b j e c t .
	Animation Object::getAnimation() const {
		return m_animation;
	}


	// S e t O b j e c t ’ s b o u n d i n g b o x .
	void Object::setBox(Box new_box) {
		m_box = new_box;
	}

	// Get O b j e c t ’ s b o u n d i n g b o x .
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