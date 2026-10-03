#pragma once
#include "Event.h"
#include "Object.h"

namespace df{

	const std::string COLLISION_EVENT = "df::collision";

	class EventCollision : public Event {

	private:
		Vector m_pos; // Where c o l l i s i o n o c c u r r e d .
		Object* m_p_obj1; // O b j e c t moving , c a u s i n g c o l l i s i o n .
		Object* m_p_obj2; // O b j e c t b e i n g c o l l i d e d w i t h .

	public:
		// C r e a t e c o l l i s i o n e v e n t a t ( 0 , 0 ) w i t h o1 and o2 NULL.
		EventCollision();

		// C r e a t e c o l l i s i o n e v e n t b e t w e e n o1 and o2 a t p o s i t i o n p .
		// O b j e c t o1 ‘ c a u s e d ’ c o l l i s i o n by moving i n t o o b j e c t o2 .
		EventCollision(Object* p_o1, Object* p_o2, Vector p);


		// S e t o b j e c t t h a t c a u s e d c o l l i s i o n .
		void setObject1(Object* p_new_o1);

		// Return o b j e c t t h a t c a u s e d c o l l i s i o n .
		Object* getObject1() const;

		// S e t o b j e c t t h a t was c o l l i d e d w i t h .
		void setObject2(Object* p_new_o2);

		// Return o b j e c t t h a t was c o l l i d e d w i t h .
		Object* getObject2() const;

		// S e t p o s i t i o n o f c o l l i s i o n .
		void setPosition(Vector new_pos);

		// Return p o s i t i o n o f c o l l i s i o n .
		Vector getPosition() const;
	};

}