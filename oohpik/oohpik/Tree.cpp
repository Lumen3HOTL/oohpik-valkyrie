#include "Tree.h"

Tree::Tree(df::Vector position) {
	setType("Tree");
	setPosition(position);
	setSolidness(df::Solidness::HARD);
	setSprite("tree");
}