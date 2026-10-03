#pragma once
#include "Event.h"
#include "Vector.h"
namespace df {

	
	const std::string MSE_EVENT = "df::mouse";
	
	// S e t o f mouse a c t i o n s r e c o g n i z e d by D r a g o n f l y .
	enum EventMouseAction {
		UNDEFINED_MOUSE_ACTION = -1,
		CLICKED,
		MOVED,
		BUTTONDOWN,
		BUTTONUP
	
	};
	
	// S e t o f mouse b u t t o n s r e c o g n i z e d by D r a g o n f l y .
	namespace Mouse {
		enum Button {
			UNDEFINED_MOUSE_BUTTON = -1,
			LEFT,
			RIGHT,
			MIDDLE,
			
		};
		
	} // end o f namespace Mouse
	
	class EventMouse : public Event {
		
		private:
			EventMouseAction m_mouse_action; // Mouse a c t i o n .
			Mouse::Button m_mouse_button; // Mouse b u t t o n .
			Vector m_mouse_xy; // Mouse ( x , y ) c o o r d i n a t e s .
			
		public:
			EventMouse();
			
			// Load mouse e v e n t ’ s a c t i o n .
			void setMouseAction(EventMouseAction new_mouse_action);
			
			// Get mouse e v e n t ’ s a c t i o n .
			EventMouseAction getMouseAction() const;
			
			// S e t mouse e v e n t ’ s b u t t o n .
			void setMouseButton(Mouse::Button new_mouse_button);

			// Get mouse e v e n t ’ s b u t t o n .
			Mouse::Button getMouseButton() const;
	
			// S e t mouse e v e n t ’ s p o s i t i o n .
			void setMousePosition(Vector new_mouse_xy);
	
			// Get mouse e v e n t ’ s p o s i t i o n .
			Vector getMousePosition() const;
	
	};
}