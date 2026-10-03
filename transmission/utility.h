#pragma once
#include "Vector.h"
#include "Box.h"
#include "Object.h"
namespace df {
	bool positionsIntersect(Vector p1, Vector p2);
	bool boxIntersectsBox(Box A, Box B);
	Box getWorldBox(const Object* p_o);
	Box getWorldBox(const Object* p_o, Vector where);
}
