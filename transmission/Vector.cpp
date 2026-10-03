
#include "Vector.h"
#include <math.h>
#include <cmath>
#include <numbers>
namespace df {
	Vector::Vector() {
		m_x = 0;
		m_y = 0;
		m_compairison_tollerance = DEFUALT_VECTOR_COMPAIRISON_TOLLERANCE;
	}


	void Vector::setCompairisonTollerances(float new_tollerance) {
		m_compairison_tollerance = new_tollerance;
	}

	float Vector::getCompairisonTollerances() const {
		return m_compairison_tollerance;
	}

	Vector::Vector(float init_x, float init_y) {
		m_x = init_x;
		m_y = init_y;
	}

	void Vector::setX(float new_x) {
		m_x = new_x;
	}

	
	float Vector::getX() const {
		return m_x;
	}

	void Vector::setY(float new_y) {
		m_y = new_y;

	}

	float Vector::getY() const {
		return m_y;
	}

	void Vector::setXY(float new_x, float new_y) {
		m_x = new_x;
		m_y = new_y;
	}

	void Vector::scale(float s) {
		m_x = m_x * s;
		m_y = m_y * s;
	}

	float Vector::getMagnitude() const {
		float precursorVal = (m_x * m_x) + (m_y * m_y);
		//safety check to prevent potential div by zero
		if (precursorVal <= 0) {
			return 0;
		}
		return sqrt(precursorVal);
	}

	void Vector::normalize() {
		float length = this->getMagnitude();
		if (length > 0) {
			m_x = m_x / length;
			m_y = m_y / length;
		}
	}

	Vector Vector::operator +(const Vector& other) const {
		Vector v; // C r e a t e new v e c t o r .
		v.setX(m_x + other.getX()); // Add x components .
		v.setY(m_y + other.getY()); // Add y components .
		return v;
	}

	Vector Vector::operator -(const Vector& other) const {
		Vector v; // C r e a t e new v e c t o r .
		v.setX(m_x - other.getX()); // sub x components .
		v.setY(m_y - other.getY()); // sub y components .
		return v;
	}

	Vector Vector::operator *(const Vector& other) const {
		Vector v; // C r e a t e new v e c t o r .
		v.setX(m_x * other.getX()); // mul x components .
		v.setY(m_y * other.getY()); // mul y components .
		return v;
	}

	Vector Vector::operator /(const Vector& other) const {
		Vector v; // C r e a t e new v e c t o r 
		v.setX(m_x / other.getX()); // div x components .
		v.setY(m_y / other.getY()); // div y components .
		return v;
	}

	//compairison operators
	bool  Vector::operator ==(const Vector& other) const {
		return ((other.getX()<m_x+m_compairison_tollerance) && (other.getX() >= m_x - m_compairison_tollerance) && (other.getY() >= m_y - m_compairison_tollerance) && (other.getY() <= m_y + m_compairison_tollerance));
	}
	bool  Vector::operator !=(const Vector& other) const {
		return ((!((other.getX() <= m_x + m_compairison_tollerance) && (other.getX() >= m_x - m_compairison_tollerance))) || (!((other.getY() >= m_y - m_compairison_tollerance) && (other.getY() <= m_y + m_compairison_tollerance))));
	}


	float Vector::getDegreeDirection() {
		Vector startVector = Vector(m_x, m_y);
		startVector.normalize();
		//thank you internet for this formula! 
		return (atan2(startVector.getX(), startVector.getY()) * (180 / std::numbers::pi));
	}

	//set a clockwise vector in degrees, and a distance, and the positions are calcualted automatically
	void Vector::directionMovement(float direction, float distance) {
		//create hte variable we will use
		float normalized_angle = 0;
		//reset the positons
		m_x = 0.0f;
		m_y = 0.0f;
		//normalize the angle
		normalized_angle= std::fmod(direction, 360.0);
		//handle negitive values
		if (normalized_angle < 0.0) {
			normalized_angle += 360.0;
		}
		//go through 9 or so common angles and set them directly, within tollerances, for speed
		if ((normalized_angle <= 0 + m_compairison_tollerance) && (normalized_angle >= 0 - m_compairison_tollerance)) {
			m_y = -distance;
			m_x = 0;
		}else if ((normalized_angle <= 45 + m_compairison_tollerance) && (normalized_angle >= 45 - m_compairison_tollerance)) {
			m_x = distance;
			m_y = -distance;
			this->normalize();
			m_x = m_x * abs(distance);
			m_y = m_y * abs(distance);
		}
		else if ((normalized_angle <= 90 + m_compairison_tollerance) && (normalized_angle >= 90 - m_compairison_tollerance)) {
			m_x = distance;
			m_y = 0;
		}
		else if ((normalized_angle <= 135 + m_compairison_tollerance) && (normalized_angle >= 135 - m_compairison_tollerance)) {
			m_x = distance;
			m_y = distance;
			this->normalize();
			m_x = m_x * abs(distance);
			m_y = m_y * abs(distance);
		}
		else if ((normalized_angle <= 180 + m_compairison_tollerance) && (normalized_angle >= 180 - m_compairison_tollerance)) {
			m_y = distance;
			m_x = 0;
		}
		else if ((normalized_angle <= 225 + m_compairison_tollerance) && (normalized_angle >= 225 - m_compairison_tollerance)) {
			m_x = -distance;
			m_y = distance;
			this->normalize();
			m_x = m_x * abs(distance);
			m_y = m_y * abs(distance);
		}
		else if ((normalized_angle <= 270 + m_compairison_tollerance) && (normalized_angle >= 270 - m_compairison_tollerance)) {
			m_x = -distance;
			m_y = 0;
		}
		else if ((normalized_angle <= 315 + m_compairison_tollerance) && (normalized_angle >= 315 - m_compairison_tollerance)) {
			m_x =  - distance;
			m_y = -distance;
			this->normalize();
			m_x = m_x * abs(distance);
			m_y = m_y * abs(distance);
		}
		else if ((normalized_angle <= 360 + m_compairison_tollerance) && (normalized_angle >= 360 - m_compairison_tollerance)) {
			m_x = 0;
			m_y = -distance;
		}
		else {
			//convert the angle to randians
			normalized_angle = (normalized_angle * std::numbers::pi) / 180.0;
			//weirdness because of how angles work in math vs how i want them to work
			m_x = std::sin(normalized_angle) * distance;
			m_y = -std::cos(normalized_angle) * distance;
		}
		
	}
}