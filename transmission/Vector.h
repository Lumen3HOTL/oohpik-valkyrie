#pragma once
#include <xhash>
#include <bit>
namespace df {
	
	const float DEFUALT_VECTOR_COMPAIRISON_TOLLERANCE = 0.001;
	class Vector {
	private:
		float m_x;
		float m_y;
		float m_compairison_tollerance;
	public:
		// Create Vector with (x, y).
		Vector(float init_x, float init_y);
		// Default 2d (x, y) is (0, 0).
		Vector();
		// Get / set horizontal component.
		void setX(float new_x);
		float getX() const;

		// Get / set vertical component.
		void setY(float new_y);
		float getY() const;

		// Set horizontal & vertical components.
		void setXY(float new_x, float new_y);

		// Return magnitude of vector.
		float getMagnitude() const;
		// Normalize vector.
		void normalize();
		
		// Scale vector.
		void scale(float s);
		
		void setCompairisonTollerances(float new_tollereance);

		float getCompairisonTollerances() const;


		// Add two Vectors, return new Vector.
		Vector operator +(const Vector & other) const;
		// subtract two Vectors, return new Vector.
		Vector operator -(const Vector& other) const;

		//multiply two vectors
		// Add two Vectors, return new Vector.
		Vector operator *(const Vector& other) const;
		// divide two vectors
		// Add two Vectors, return new Vector.
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