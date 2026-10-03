#pragma once

namespace df {
	const float DEFUALT_VECTOR_COMPAIRISON_TOLLERANCE = 0.001;
	class Vector {
	private:
		float m_x;
		float m_y;
		float m_compairison_tollerance;
	public:
		// C r e a t e V e c to r w i t h ( x , y ) .
		Vector(float init_x, float init_y);
		// D e f a u l t 2 d ( x , y ) i s ( 0 , 0 ) .
		Vector();
		// Get / s e t h o r i z o n t a l component .
		void setX(float new_x);
		float getX() const;

		// Get / s e t v e r t i c a l component .
		void setY(float new_y);
		float getY() const;

		// S e t h o r i z o n t a l & v e r t i c a l components .
		void setXY(float new_x, float new_y);

		// Return m a g n i tu d e o f v e c t o r .
		float getMagnitude() const;
		// N o r m a l i z e v e c t o r .
		void normalize();
		
		// S c a l e v e c t o r .
		void scale(float s);
		
		void setCompairisonTollerances(float new_tollereance);

		float getCompairisonTollerances() const;


		// Add two V e c to r s , r e t u r n new V e c to r .
		Vector operator +(const Vector & other) const;
		// subtract two V e c to r s , r e t u r n new V e c to r .
		Vector operator -(const Vector& other) const;

		//multiply two vectors
		// Add two V e c to r s , r e t u r n new V e c to r .
		Vector operator *(const Vector& other) const;
		// divide two vectors
		// Add two V e c to r s , r e t u r n new V e c to r .
		Vector operator /(const Vector& other) const;

		//compairison operators
		bool operator ==(const Vector& other) const;
		bool operator !=(const Vector& other) const;
		//set the internal representation to an xy value closest to the movement of a certain distance in a direction;
		void directionMovement(float direction, float distance);

		//get the vector's direction in degrees
		float getDegreeDirection();


	};
}