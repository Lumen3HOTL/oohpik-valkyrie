#include "Box.h"

namespace df {
	// C r e a t e b o x w i t h ( 0 , 0 ) f o r t h e c o r n e r , and 0 f o r h o r i z and v e r t .
	Box::Box() {
		m_corner = Vector();
		m_horizontal = 0;
		m_vertical = 0;
	}

	// C r e a t e b o x w i t h an upper − l e f t c o r n e r , h o r i z and v e r t s i z e s .
	Box::Box(Vector init_corner, float init_horizontal, float init_vertical) {
		m_corner = init_corner;
		m_horizontal = init_horizontal;
		m_vertical = init_vertical;
	}

	// S e t u p p e r l e f t c o r n e r o f b o x .
	void Box::setCorner(Vector new_corner) {
		m_corner = new_corner;
	}

	// Get u p p e r l e f t c o r n e r o f b o x .
	Vector Box::getCorner() const {
		return m_corner;
	}

	// S e t h o r i z o n t a l s i z e o f b o x .
	void Box::setHorizontal(float new_horizontal) {
		m_horizontal = new_horizontal;
	}

	// Get h o r i z o n t a l s i z e o f b o x .
	float Box::getHorizontal() const {
		return m_horizontal;
	}

	// S e t v e r t i c a l s i z e o f b o x .

	void Box::setVertical(float new_vertical) {
		m_vertical = new_vertical;
	}

	// Get v e r t i c a l s i z e o f b o x .
	float Box::getVertical() const {
		return m_vertical;
	}
}