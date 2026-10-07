#include "Tree.h"

namespace ookpik {
	// Creates a new tree at a given position, used for specific placement of trees
	Tree::Tree(df::Vector position) {
		setType("Tree");
		setPosition(position);
		setSolidness(df::Solidness::HARD);
		setSprite("tree");
	}

	// Creates a new tree but does not change its positition from spawn, used primarily for mapgen
	Tree::Tree() {
		setType("Tree");
		setSolidness(df::Solidness::HARD);
		setSprite("tree");
	}
}