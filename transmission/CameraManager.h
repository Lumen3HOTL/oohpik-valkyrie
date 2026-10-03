#pragma once
#include <vector>
#include "Camera.h"
#include "DisplayManager.h"
#include <string>

namespace df {
	class CameraManager : public Manager {

		private:
			CameraManager(); // P r i v a t e ( a s i n g l e t o n ) .
			CameraManager(CameraManager const&); // Don ’ t a l l o w copy .
			void operator =(CameraManager const&); // Don ’ t a l l o w a s s i g n m e n t
			int m_currentCameraIndex;
			std::vector<Camera> m_cameras;
		public:
			static CameraManager& getInstance();
			// Open g r a p h i c s window , r e a d y f o r t e x t −b a s e d d i s p l a y .
				// Return 0 i f ok , e l s e −1.
			int startUp();

			// C l o s e g r a p h i c s window .
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