#include "utility.h"
#include <math.h>
#include "Object.h"
#include "Vector.h"
namespace df {
	bool positionsIntersect(Vector p1, Vector p2) {
		return ((fabs(p1.getX() - p2.getX()) <= 1) && (fabs(p1.getY() - p2.getY()) <= 1));
	}

	// C o n v e r t r e l a t i v e b o u n d i n g Box f o r O b j e c t t o a b s o l u t e w o r l d Box .
	Box getWorldBox(const Object* p_o){

		Box box = p_o->getBox();
		Vector corner = box.getCorner();

		corner.setX(corner.getX() + p_o->getPosition().getX());
		corner.setY(corner.getY() + p_o->getPosition().getY());
		box.setCorner(corner);

		return box;
	}

	Box getWorldBox(const Object* p_o, Vector where){
		Box box = p_o->getBox();
		Vector corner = box.getCorner();

		corner.setX(corner.getX() + where.getX());
		corner.setY(corner.getY() + where.getY());
		box.setCorner(corner);
		return box;
	}

	bool boxIntersectsBox(Box A, Box B) {
		//extract the corners
		float aLeft = A.getCorner().getX();
		float aRight = aLeft + A.getHorizontal();
		float aTop = A.getCorner().getY();
		float aBottom = aTop + A.getVertical();

		float bLeft = B.getCorner().getX();
		float bRight = bLeft + B.getHorizontal();
		float bTop = B.getCorner().getY();
		float bBottom = bTop + B.getVertical();


		//one liner logic go brrrrrrrr
		return ((aLeft < bRight) && (aRight > bLeft) && (aTop < bBottom) && (aBottom > bTop));
	}
	
}