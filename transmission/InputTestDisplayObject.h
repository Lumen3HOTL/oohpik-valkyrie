#pragma once
#include "Object.h"
#include <string>
#include <vector>
#include "EventStep.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "Color.h"
#include "Clock.h"
#include "DisplayManager.h"

namespace test {
	//enum to get away with one set sprite function
	enum inputTestDisplayEditSelection {
		UNDEFINED_EDIT_SELECTION=-1,
		EDIT_KEY_DOWN_SPRITE=0,
		EDIT_MOUSE_MOVE_SPRITE,
		EDIT_MOUSE_BUTTON_DOWN_SPRITE,
		EDIT_MOUSE_BUTTON_UP_SPRITE,
		EDIT_MOUSE_CLICK_SPRITE,
	
	};
	//a very useful diagnostics object
	class InputTestDisplayObject :public  df::Object {
	private:
		//framecounter
		unsigned int m_frameCounter;
		//the different strings this object draws
		std::string m_key_down_sprite;
		std::string m_mouse_move_sprite;
		std::string m_mouse_button_up_sprite;
		std::string m_mouse_button_down_sprite;
		std::string m_mouse_click_sprite;
		//the currently down mouse buttons
		std::vector<bool> m_downMouseButtons;
		//the names of the keyboard keys
		std::vector<std::string> m_key_names;
		//the names of the buttons
		std::vector<std::string> m_button_names;
		//the current text color
		df::Color m_color;
		//the current down keys
		std::vector<bool> m_downKeyButtons;
		//the clocks for storing the times since last click
		std::vector<df::Clock> m_timeSinceLastClicks;
		//the current mouse position
		df::Vector m_mousePos;

		//clock for timeing
		df::Clock m_timer;
		df::Clock m_inter_frame_stopwatch;

		std::string m_inter_update_time_ms;

		bool m_in_engine;

		int setSprite(inputTestDisplayEditSelection selection, std::string newTexture);
	public:

		void setInEngine(bool newState);

		InputTestDisplayObject();

		~InputTestDisplayObject();

		int draw();

		int setColor(df::Color newcolor);

		

		int eventHandler(const df::Event* p_e);
	};
}
