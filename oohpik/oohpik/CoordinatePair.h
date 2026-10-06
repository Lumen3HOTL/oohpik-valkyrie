#pragma once
#include "Vector.h"

namespace ookpik {

	class CoordinatePair {
	private:
		df::Vector m_point0;
		df::Vector m_point1;
	public:
		CoordinatePair();
		CoordinatePair(df::Vector point0, df::Vector point1);
		df::Vector getPoint0()const;
		df::Vector getPoint1()const;

		void setPoint0(df::Vector new_point0);
		void setPoint1(df::Vector new_point1);
	};
}