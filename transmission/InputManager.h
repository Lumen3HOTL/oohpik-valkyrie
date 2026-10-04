#pragma once
#include "Manager.h"
#include "EventKeyboard.h"
#include "Clock.h"
namespace df {

	
	const int MOUSE_CLICK_TIME=5;

	class InputManager : public Manager {
	
	private:
		InputManager(); // Private (a singleton).
		InputManager(InputManager const&); // Don't allow copy.
		void operator =(InputManager const&); // Don't allow assignment
		Clock m_leftMouseButtonFrameCounter;
		Clock m_middleMouseButtonFrameCounter;
		Clock m_rightMouseButtonFrameCounter;
		Clock m_undefinedMouseButtonFrameCounter;
		bool m_leftMouseButtonActive;
		bool m_middleMouseButtonActive;
		bool m_rightMouseButtonActive;
		bool m_undefinedMouseButtonActive;
		int m_mouseClickTollerance;
	
	public:
		// Get the one and only instance of the InputManager.
			static InputManager & getInstance();
		
			// Get window ready to capture input.
			// Return 0 if ok, else return -1.
			int startUp();
		
			// Revert back to normal window mode.
			void shutDown();
		
			// Get input from the keyboard and mouse.
			// Pass event along to all Objects.
			void getInput();
		
	};
}
