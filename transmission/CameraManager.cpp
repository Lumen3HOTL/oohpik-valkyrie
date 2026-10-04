#include "CameraManager.h"
#include "WorldManager.h"
namespace df {
	CameraManager::CameraManager() {
		this->setType("CameraManager");
		m_currentCameraIndex = 0;
		m_cameras = std::vector<Camera>();
	}

	CameraManager& CameraManager::getInstance() {
		static CameraManager CameraMan = CameraManager();
		return CameraMan;
	}

	int CameraManager::startUp() {
		if (this->isStarted()) {
			return -1;
		}

		m_currentCameraIndex = 0;
		m_cameras.clear();

		return Manager::startUp();
	}

	// Close graphics window.
	void CameraManager::shutDown() {
		m_currentCameraIndex = 0;
		m_cameras.clear();
		Manager::shutDown();
	}

	Camera* CameraManager::getCurrentCamera()  {
		if (this->isStarted()) {
			Camera* cameraPtr = &m_cameras[m_currentCameraIndex];
			return cameraPtr;
		}
		return nullptr;
	}


	int CameraManager::setCurrentCamera(std::string camera_name) {
		if (this->isStarted()) {
	
			for (int index = 0; index < m_cameras.size(); index++) {
				if (m_cameras.at(index).getCameraName().compare(camera_name) == 0) {
					m_currentCameraIndex = index;
					WorldManager::getInstance().massFrustromCheck();
					return 0;
				}
			}

			return -1;
		}
		return -1;
	}

	int CameraManager::addCamera(std::string label) {
		if (this->isStarted()) {
			

			for (int search = 0; search < m_cameras.size(); search++) {
				if (m_cameras.at(search).getCameraName().compare(label) == 0) {
					return -1;
				}
			}

			Camera tempCam = Camera(label);
			m_cameras.push_back(tempCam);
			return 0;
		}
		return -1;
	}

	int CameraManager::addCamera(std::string label, Vector init_pos) {
		if (this->isStarted()) {


			for (int search = 0; search < m_cameras.size(); search++) {
				if (m_cameras.at(search).getCameraName().compare(label) == 0) {
					return -1;
				}
			}

			Camera tempCam = Camera(label,init_pos);
			m_cameras.push_back(tempCam);
		}
		return -1;
	}



	int CameraManager::removeCamera(std::string camera_name) {
		if (this->isStarted()) {
			if (m_cameras.empty()) {
				return -1;
			}
			int found = -1;
			for (int search = m_cameras.size() - 1; search >= 0; search--) {
				if (m_cameras[search].getCameraName().compare(camera_name) == 0) {
					m_cameras[search] = m_cameras[m_cameras.size() - 1];
					m_cameras.pop_back();
					found = 0;
					if (m_currentCameraIndex == search) {
						m_currentCameraIndex = 0;
					}
				}
			}
			return found;
		}
		return -1;
	}

	

	Camera* CameraManager::getCamera(std::string camera_name) {
		if (this->isStarted()) {

			for (int index = 0; index < m_cameras.size(); index++) {
				if (m_cameras.at(index).getCameraName().compare(camera_name) == 0) {
					return &m_cameras.at(index);
					
				}
			}

			
		}
		return nullptr;
	}

	int CameraManager::getCount() const {
		if (this->isStarted()) {
			return m_cameras.size();
		}
		return -1;
	}


	bool CameraManager::isEmpty() const {
		if (this->isStarted()) {
			return m_cameras.empty();
		}
		return true;
	}

	int CameraManager::clear() {
		if (this->isStarted()) {
			m_currentCameraIndex = 0;
			m_cameras.clear();
		}
		return -1;
	}
}