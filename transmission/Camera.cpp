#include "Camera.h"
#include "WorldManager.h"
#include "CameraManager.h"
namespace df {
	

	Camera::Camera() {
		m_cameraPos = Vector();
		m_name = "UNDEFINED_CAMERA";

	}
	Camera::Camera(std::string label) {
		m_cameraPos = Vector();
		m_name = label;
	}
	Camera::Camera(std::string label, Vector pos) {
		m_cameraPos = pos;
		m_name = label;
	}
	Vector Camera::getCameraPos()const {
		return m_cameraPos;
	}
	void Camera::setCameraPos(Vector new_pos) {
		m_cameraPos = new_pos;
		
		CameraManager& cam = CameraManager::getInstance();
		if (!cam.isEmpty()) {
			if (cam.getCurrentCamera()->getCameraName().compare(m_name) == 0) {
				WorldManager::getInstance().massFrustromCheck();
			}
		}
	}
	std::string Camera::getCameraName()const {
		return m_name;
	}
	void Camera::setCameraName(std::string new_label) {
		m_name = new_label;
	}
	
}