#pragma once
#include <string>
#include "Vector.h"

namespace df {
	class Camera {
	private:
		Vector m_cameraPos;
		std::string m_name;

	public:
		Camera();
		Camera(std::string label);
		Camera(std::string label, Vector pos);
		Vector getCameraPos()const;
		void setCameraPos(Vector new_pos);
		std::string getCameraName()const;
		void setCameraName(std::string new_label);
	};

}