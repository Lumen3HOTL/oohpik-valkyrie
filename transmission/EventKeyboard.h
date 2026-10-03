#pragma once
#include "Event.h"
#include <SFML/Window.hpp>
#include <vector>
namespace df {

	


	const std::string KEYBOARD_EVENT = "df::Keyboard";
	
		// Types o f k e y b o a r d a c t i o n s D r a g o n f l y r e c o g n i z e s .
	enum EventKeyboardAction {
		UNDEFINED_KEYBOARD_ACTION = -1, // U n d e f i n e d .
		KEY_PRESSED, // Was down .
		KEY_RELEASED, // Was r e l e a s e d .
		
	};
	
		// Keys D r a g o n f l y r e c o g n i z e s .
	namespace Keyboard {
		enum Key {
			UNDEFINED_KEY = -1,
			SPACE, RETURN, ESCAPE, TAB, LEFTARROW, RIGHTARROW, UPARROW, DOWNARROW,
			PAUSE, MINUS, PLUS, TILDE, PERIOD, COMMA, SLASH, LEFTCONTROL,
			RIGHTCONTROL, LEFTSHIFT, RIGHTSHIFT, F1, F2, F3, F4, F5, F6, F7, F8,
			F9, F10, F11, F12, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q,
			R, S, T, U, V, W, X, Y, Z, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7,
			NUM8, NUM9, NUM0, BACKSPACE, EQUAL, BACKSLASH, LEFTALT, RIGHTALT, 
			LEFTBRACKET, RIGHTBRACKET, SEMICOLON, APOSTROPHE, HYPHEN, PAGEUP,
			PAGEDOWN, END, HOME, INSERT, KDELETE, STAR, NUMPAD0, NUMPAD1, NUMPAD2,
			NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9,
			
		};
		
	} // end o f namespace Keyboard
	//a nice pair of program generated lookup table for the different keycodes to speed up conversion (program is "key lookup table generator.py")
	const std::array SF_TO_DF_KEYCODE_LUT = { Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::A, Keyboard::Key::B, Keyboard::Key::C, Keyboard::Key::D, Keyboard::Key::E, Keyboard::Key::F, Keyboard::Key::G, Keyboard::Key::H, Keyboard::Key::I, Keyboard::Key::J, Keyboard::Key::K, Keyboard::Key::L, Keyboard::Key::M, Keyboard::Key::N, Keyboard::Key::O, Keyboard::Key::P, Keyboard::Key::Q, Keyboard::Key::R, Keyboard::Key::S, Keyboard::Key::T, Keyboard::Key::U, Keyboard::Key::V, Keyboard::Key::W, Keyboard::Key::X, Keyboard::Key::Y, Keyboard::Key::Z, Keyboard::Key::NUM0, Keyboard::Key::NUM1, Keyboard::Key::NUM2, Keyboard::Key::NUM3, Keyboard::Key::NUM4, Keyboard::Key::NUM5, Keyboard::Key::NUM6, Keyboard::Key::NUM7, Keyboard::Key::NUM8, Keyboard::Key::NUM9, Keyboard::Key::ESCAPE, Keyboard::Key::LEFTCONTROL, Keyboard::Key::LEFTSHIFT, Keyboard::Key::LEFTALT, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::RIGHTCONTROL, Keyboard::Key::RIGHTSHIFT, Keyboard::Key::RIGHTALT, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::LEFTBRACKET, Keyboard::Key::RIGHTBRACKET, Keyboard::Key::SEMICOLON, Keyboard::Key::COMMA, Keyboard::Key::PERIOD, Keyboard::Key::APOSTROPHE, Keyboard::Key::SLASH, Keyboard::Key::BACKSLASH, Keyboard::Key::TILDE, Keyboard::Key::EQUAL, Keyboard::Key::HYPHEN, Keyboard::Key::SPACE, Keyboard::Key::RETURN, Keyboard::Key::BACKSPACE, Keyboard::Key::TAB, Keyboard::Key::PAGEUP, Keyboard::Key::PAGEDOWN, Keyboard::Key::END, Keyboard::Key::HOME, Keyboard::Key::INSERT, Keyboard::Key::KDELETE, Keyboard::Key::PLUS, Keyboard::Key::MINUS, Keyboard::Key::STAR, Keyboard::Key::SLASH, Keyboard::Key::LEFTARROW, Keyboard::Key::RIGHTARROW, Keyboard::Key::UPARROW, Keyboard::Key::DOWNARROW, Keyboard::Key::NUMPAD0, Keyboard::Key::NUMPAD1, Keyboard::Key::NUMPAD2, Keyboard::Key::NUMPAD3, Keyboard::Key::NUMPAD4, Keyboard::Key::NUMPAD5, Keyboard::Key::NUMPAD6, Keyboard::Key::NUMPAD7, Keyboard::Key::NUMPAD8, Keyboard::Key::NUMPAD9, Keyboard::Key::F1, Keyboard::Key::F2, Keyboard::Key::F3, Keyboard::Key::F4, Keyboard::Key::F5, Keyboard::Key::F6, Keyboard::Key::F7, Keyboard::Key::F8, Keyboard::Key::F9, Keyboard::Key::F10, Keyboard::Key::F11, Keyboard::Key::F12, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::UNDEFINED_KEY, Keyboard::Key::PAUSE};
	const std::array DF_TO_SF_KEYCODE_LUT={ sf::Keyboard::Key::Unknown, sf::Keyboard::Key::Space, sf::Keyboard::Key::Enter, sf::Keyboard::Key::Escape, sf::Keyboard::Key::Tab, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::Pause, sf::Keyboard::Key::Subtract, sf::Keyboard::Key::Add, sf::Keyboard::Key::Grave, sf::Keyboard::Key::Period, sf::Keyboard::Key::Comma, sf::Keyboard::Key::Slash, sf::Keyboard::Key::LControl, sf::Keyboard::Key::RControl, sf::Keyboard::Key::LShift, sf::Keyboard::Key::RShift, sf::Keyboard::Key::F1, sf::Keyboard::Key::F2, sf::Keyboard::Key::F3, sf::Keyboard::Key::F4, sf::Keyboard::Key::F5, sf::Keyboard::Key::F6, sf::Keyboard::Key::F7, sf::Keyboard::Key::F8, sf::Keyboard::Key::F9, sf::Keyboard::Key::F10, sf::Keyboard::Key::F11, sf::Keyboard::Key::F12, sf::Keyboard::Key::A, sf::Keyboard::Key::B, sf::Keyboard::Key::C, sf::Keyboard::Key::D, sf::Keyboard::Key::E, sf::Keyboard::Key::F, sf::Keyboard::Key::G, sf::Keyboard::Key::H, sf::Keyboard::Key::I, sf::Keyboard::Key::J, sf::Keyboard::Key::K, sf::Keyboard::Key::L, sf::Keyboard::Key::M, sf::Keyboard::Key::N, sf::Keyboard::Key::O, sf::Keyboard::Key::P, sf::Keyboard::Key::Q, sf::Keyboard::Key::R, sf::Keyboard::Key::S, sf::Keyboard::Key::T, sf::Keyboard::Key::U, sf::Keyboard::Key::V, sf::Keyboard::Key::W, sf::Keyboard::Key::X, sf::Keyboard::Key::Y, sf::Keyboard::Key::Z, sf::Keyboard::Key::Num1, sf::Keyboard::Key::Num2, sf::Keyboard::Key::Num3, sf::Keyboard::Key::Num4, sf::Keyboard::Key::Num5, sf::Keyboard::Key::Num6, sf::Keyboard::Key::Num7, sf::Keyboard::Key::Num8, sf::Keyboard::Key::Num9, sf::Keyboard::Key::Num0, sf::Keyboard::Key::Backspace, sf::Keyboard::Key::Equal, sf::Keyboard::Key::Backslash, sf::Keyboard::Key::LAlt, sf::Keyboard::Key::RAlt, sf::Keyboard::Key::LBracket, sf::Keyboard::Key::RBracket, sf::Keyboard::Key::Semicolon, sf::Keyboard::Key::Apostrophe, sf::Keyboard::Key::Hyphen, sf::Keyboard::Key::PageUp, sf::Keyboard::Key::PageDown, sf::Keyboard::Key::End, sf::Keyboard::Key::Home, sf::Keyboard::Key::Insert, sf::Keyboard::Key::Delete, sf::Keyboard::Key::Multiply, sf::Keyboard::Key::Numpad0, sf::Keyboard::Key::Numpad1, sf::Keyboard::Key::Numpad2, sf::Keyboard::Key::Numpad3, sf::Keyboard::Key::Numpad4, sf::Keyboard::Key::Numpad5, sf::Keyboard::Key::Numpad6, sf::Keyboard::Key::Numpad7, sf::Keyboard::Key::Numpad8, sf::Keyboard::Key::Numpad9};

	
	Keyboard::Key SFKeycodeToDragonflyKeycode(sf::Keyboard::Key sfKey);
	sf::Keyboard::Key DragonflyKeycodeToSFKeycode(Keyboard::Key dfKey);

	class EventKeyboard : public Event {
		
		private:
			Keyboard::Key m_key_val; // Key v a l u e .
			EventKeyboardAction m_keyboard_action; // Key a c t i o n .
		
		public:
			EventKeyboard();
			
			// S e t k e y i n e v e n t .
			void setKey(Keyboard::Key new_key);
			
			// Get k e y from e v e n t .
			Keyboard::Key getKey() const;
			
			// S e t k e y b o a r d e v e n t a c t i o n .
			void setKeyboardAction(EventKeyboardAction new_action);
			
			// Get k e y b o a r d e v e n t a c t i o n .
			EventKeyboardAction getKeyboardAction() const;
			
	};
}