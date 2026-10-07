#include "CoordinatePair.h"

namespace ookpik {
	CoordinatePair::CoordinatePair() {
		m_point0 = df::Vector();
		m_point1 = df::Vector();
	}
	CoordinatePair::CoordinatePair(df::Vector point0, df::Vector point1) {
		m_point0 = point0;
		m_point1 = point1;
	}
	df::Vector CoordinatePair::getPoint0()const {
		return m_point0;
	}
	df::Vector CoordinatePair::getPoint1()const {
		return m_point1;
	}

	void CoordinatePair::setPoint0(df::Vector new_point0) {
		m_point0 = new_point0;
	}
	void CoordinatePair::setPoint1(df::Vector new_point1) {
		m_point1 = new_point1;
	}
}