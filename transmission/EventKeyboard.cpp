#include "EventKeyboard.h"

namespace df {



	Keyboard::Key SFKeycodeToDragonflyKeycode(sf::Keyboard::Key sfKey) {
		unsigned int index=((unsigned int)sfKey) + 1;
		if (index < SF_TO_DF_KEYCODE_LUT.size()) {
			return SF_TO_DF_KEYCODE_LUT[index];
		}
		return Keyboard::UNDEFINED_KEY;
	}

	sf::Keyboard::Key DragonflyKeycodeToSFKeycode(Keyboard::Key dfKey) {
		unsigned int index = ((unsigned int)dfKey) + 1;
		if (index < DF_TO_SF_KEYCODE_LUT.size()) {
			return DF_TO_SF_KEYCODE_LUT[index];
		}
		return sf::Keyboard::Key::Unknown;
	}



	EventKeyboard::EventKeyboard() {
		m_key_val = Keyboard::UNDEFINED_KEY;
		m_keyboard_action = UNDEFINED_KEYBOARD_ACTION;
		this->setType(KEYBOARD_EVENT);
	}

	Keyboard::Key EventKeyboard::getKey() const {
		return m_key_val;

	}

	EventKeyboardAction EventKeyboard::getKeyboardAction() const {
		return m_keyboard_action;
	}

	void EventKeyboard::setKey(Keyboard::Key new_key) {
		m_key_val = new_key;

	}

	void EventKeyboard::setKeyboardAction(EventKeyboardAction new_action) {
		m_keyboard_action = new_action;
	}
}