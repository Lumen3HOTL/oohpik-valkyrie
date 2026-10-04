#pragma once
#include <vector>
#include "Camera.h"
#include "DisplayManager.h"
#include <string>

namespace df {
	class CameraManager : public Manager {

		private:
			CameraManager(); // Private (a singleton).
			CameraManager(CameraManager const&); // Don't allow copy.
			void operator =(CameraManager const&); // Don't allow assignment
			int m_currentCameraIndex;
			std::vector<Camera> m_cameras;
		public:
			static CameraManager& getInstance();
			// Open graphics window, ready for text-based display.
				// Return 0 if ok, else -1.
			int startUp();

			// Close graphics window.
			void shutDown();

			Camera* getCurrentCamera();



			int setCurrentCamera(std::string camera_name);

			int addCamera(std::string label);

			int addCamera(std::string label, Vector init_pos);



			int removeCamera(std::string camera_name);



			Camera* getCamera(std::string camera_name);

			int getCount() const;
			bool isEmpty() const;
			int clear();
	};
}