#pragma once
#include "Manager.h"
#include "EventKeyboard.h"
#include "Clock.h"
namespace df {

	
	const int MOUSE_CLICK_TIME=5;

	class InputManager : public Manager {
	
	private:
		InputManager(); // P r i v a t e ( a s i n g l e t o n ) .
		InputManager(InputManager const&); // Don ’ t a l l o w copy .
		void operator =(InputManager const&); // Don ’ t a l l o w a s s i g n m e n t
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
		// Get t h e one and o n l y i n s t a n c e o f t h e InputManager .
			static InputManager & getInstance();
		
			// Get window r e a d y t o c a p t u r e i n p u t .
			// Return 0 i f ok , e l s e r e t u r n −1.
			int startUp();
		
			// R e v e r t b a c k t o normal window mode .
			void shutDown();
		
			// Get i n p u t from t h e k e y b o a r d and mouse .
			// Pass e v e n t a l o n g t o a l l O b j e c t s .
			void getInput();
		
	};
}
