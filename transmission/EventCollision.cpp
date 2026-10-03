#include "EventCollision.h"

namespace df {
	// C r e a t e c o l l i s i o n e v e n t a t ( 0 , 0 ) w i t h o1 and o2 NULL.
	EventCollision::EventCollision() {
		m_pos = Vector();
		m_p_obj1 = nullptr;
		m_p_obj2 = nullptr;
		this->setType(COLLISION_EVENT);

	}

	// C r e a t e c o l l i s i o n e v e n t b e t w e e n o1 and o2 a t p o s i t i o n p .
	// O b j e c t o1 ‘ c a u s e d ’ c o l l i s i o n by moving i n t o o b j e c t o2 .
	EventCollision::EventCollision(Object* p_o1, Object* p_o2, Vector p) {
		m_pos = p;
		m_p_obj1 = p_o1;
		m_p_obj2 = p_o2;
		this->setType(COLLISION_EVENT);
	}


	// S e t o b j e c t t h a t c a u s e d c o l l i s i o n .
	void EventCollision::setObject1(Object* p_new_o1) {
		m_p_obj1 = p_new_o1;
	}

	// Return o b j e c t t h a t c a u s e d c o l l i s i o n .
	Object* EventCollision::getObject1() const {
		return m_p_obj1;
	}

	// S e t o b j e c t t h a t was c o l l i d e d w i t h .
	void EventCollision::setObject2(Object* p_new_o2) {
		m_p_obj2 = p_new_o2;
	}

	// Return o b j e c t t h a t was c o l l i d e d w i t h .
	Object* EventCollision::getObject2() const {
		return m_p_obj2;
	}

	// S e t p o s i t i o n o f c o l l i s i o n .
	void EventCollision::setPosition(Vector new_pos) {
		m_pos = new_pos;
	}

	// Return p o s i t i o n o f c o l l i s i o n .
	Vector EventCollision::getPosition() const {
		return m_pos;
	}
}