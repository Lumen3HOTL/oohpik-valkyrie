#pragma once
#include "Object.h"
#include "Animation.h"
#include <string>
#include "DisplayManager.h"
#include "LogManager.h"


namespace test {
	class AnimationTestObject :public  df::Object {
	private:
		


	public:
		AnimationTestObject();

		~AnimationTestObject();

		int draw();



	};
}