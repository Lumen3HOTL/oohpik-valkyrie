#include "AnimationTestobject.h"
#include <iostream>
namespace test {
	AnimationTestObject::AnimationTestObject(){

	}

	AnimationTestObject::~AnimationTestObject() {

	}

	int AnimationTestObject::draw() {
		if (this->getVisible()) {
			if ((this->getAnimation().getSprite() == nullptr)) {
				return df::DisplayManager::getInstance().drawString(this->getPosition(), "<no-anim>", df::CENTER_JUSTIFIED, df::COLOR_DEFAULT, df::CENTER_ALLIGNED);
			}
			else {
				//i hate this code but its the only way to do this with the required architecture. separation of concern? never heard of it!
				df::Animation temp = this->getAnimation();

				int error = temp.draw(this->getPosition());
			
				this->setAnimation(temp);
				return error;
			}
			
		}
		return 0;
	}
}


