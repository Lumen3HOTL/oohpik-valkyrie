#include "InputTestDisplayObject.h"
#include "LogManager.h"
#include "GameManager.h"
namespace test {
	int InputTestDisplayObject::setSprite(inputTestDisplayEditSelection selection, std::string newTexture) {
		//self explanitory, depending on the enum, set the corisponding texture
		switch (selection) {

		case EDIT_KEY_DOWN_SPRITE:
			m_key_down_sprite = newTexture;
			break;
		case EDIT_MOUSE_MOVE_SPRITE:
			m_mouse_move_sprite = newTexture;
			break;
		case EDIT_MOUSE_CLICK_SPRITE:
			m_mouse_click_sprite = newTexture;
			break;
		case EDIT_MOUSE_BUTTON_UP_SPRITE:
			m_mouse_button_up_sprite = newTexture;
			break;
		case EDIT_MOUSE_BUTTON_DOWN_SPRITE:
			m_mouse_button_down_sprite = newTexture;
			break;
		
		default:
			return -1;
		}
		return 0;
	}

	InputTestDisplayObject::InputTestDisplayObject() {
		//init the memeber variables
		m_in_engine = false;
		this->setType("inputTestDisplayObject");
		m_frameCounter = 0;

		m_key_down_sprite="no keys";

		m_mouse_move_sprite="no movement";
		m_mouse_button_up_sprite="no buttons down";
		m_mouse_button_down_sprite="all buttons up";
		m_mouse_click_sprite="no clicks";
		m_color=df::WHITE;
		
		m_downKeyButtons = std::vector<bool>(df::DF_TO_SF_KEYCODE_LUT.size());
		m_downMouseButtons = std::vector<bool>(4);
		m_key_names = { "UNDEFINED_KEY", "SPACE", "RETURN", "ESCAPE", "TAB", "LEFTARROW", "RIGHTARROW", "UPARROW", "DOWNARROW", "PAUSE", "MINUS", "PLUS", "TILDE", "PERIOD", "COMMA", "SLASH", "LEFTCONTROL", "RIGHTCONTROL", "LEFTSHIFT", "RIGHTSHIFT", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "NUM7", "NUM8", "NUM9", "NUM0", "BACKSPACE", "EQUAL", "BACKSLASH", "LEFTALT", "RIGHTALT", "LEFTBRACKET", "RIGHTBRACKET", "SEMICOLON", "APOSTROPHE", "HYPHEN", "PAGEUPPAGEDOWN", "END", "HOME", "INSERT", "KDELETE", "STAR", "NUMPAD0", "NUMPAD1", "NUMPAD2", "NUMPAD3", "NUMPAD4", "NUMPAD5", "NUMPAD6", "NUMPAD7", "NUMPAD8", "NUMPAD9", };
		m_button_names = { "LEFT","MIDDLE","RIGHT","UNDEFINED" };
		
		m_timeSinceLastClicks=std::vector<df::Clock>(4);
		m_mousePos=df::Vector();
		m_timer = df::Clock();
		
		this->setSolidness(df::SPECTRAL);
	}

	InputTestDisplayObject::~InputTestDisplayObject() {

	}
	void InputTestDisplayObject::setInEngine(bool newState) {
		m_in_engine = newState;
	}
	int InputTestDisplayObject::setColor(df::Color newcolor) {
		//self explanitory
		m_color = newcolor;
		return 0;
	}

	int InputTestDisplayObject::draw() {
		//if we are visible
		if (this->getVisible()) {
			//get the display manager
			df::DisplayManager& display = df::DisplayManager::getInstance();
			//grab teh current positon
			//draw each sprite in sequence, error checking and incrememting y as we go along
			int error = 0;
			if (m_in_engine) {
				df::GameManager& gm = df::GameManager::getInstance();
				error = display.drawString(this->getPosition(), std::string("input diagnostics:\ntime since start: ").append(std::to_string(m_timer.split() / 1000)).append("ms,  frame: ").append(std::to_string(m_frameCounter)).append(",\nlast frame time: ").append(std::to_string(gm.getLastFrameTime() / 1000)).append(", last delta time: ").append(std::to_string(gm.getLastDeltaTime() / 1000)).append(", input state:\n").append(m_key_down_sprite).append("\n").append(m_mouse_move_sprite).append("\n").append(m_mouse_button_down_sprite).append("\n").append(m_mouse_button_up_sprite).append("\n").append(m_mouse_click_sprite), df::CENTER_JUSTIFIED, m_color);

			}
			else {
				 error= display.drawString(this->getPosition(), std::string("input diagnostics:\ntime since start: ").append(std::to_string(m_timer.split() / 1000)).append("ms,  frame: ").append(std::to_string(m_frameCounter)).append(", input state:\n").append(m_key_down_sprite).append("\n").append(m_mouse_move_sprite).append("\n").append(m_mouse_button_down_sprite).append("\n").append(m_mouse_button_up_sprite).append("\n").append(m_mouse_click_sprite), df::CENTER_JUSTIFIED, m_color);

			}
			

			if (error < 0) {
				return -1;
			}
		}
		
		return 0;
	}

	int InputTestDisplayObject::eventHandler(const df::Event* p_e) {
		//on step
		if (p_e->getType().compare(df::STEP_EVENT)==0 ){
			//reset the timer
			if (m_frameCounter == 0) {
				m_timer.delta();
			}
			
			
			//update the click and mouse button up/down textures
			//update the on click texture
			std::string newClick = "time since last click: ";
			newClick = newClick.append("left: ").append(std::to_string(m_timeSinceLastClicks[0].split() / 1000)).append("ms, ");
			newClick = newClick.append("middle: ").append(std::to_string(m_timeSinceLastClicks[1].split() / 1000)).append("ms,\n");
			newClick = newClick.append("right: ").append(std::to_string(m_timeSinceLastClicks[2].split() / 1000)).append("ms, ");
			newClick = newClick.append("undefined: ").append(std::to_string(m_timeSinceLastClicks[3].split() / 1000)).append("ms ");
			this->setSprite(EDIT_MOUSE_CLICK_SPRITE, newClick);
			//update the mouse up and down textures
			//base texture
			std::string newUp = "up buttons: ";
			std::string newDown = "down buttons: ";
			//loop through the vector of states and add the button name ot the corisponding string
			for (int i = 0; i < m_downMouseButtons.size(); i++) {
				if (m_downMouseButtons[i]) {
					newDown = newDown.append(m_button_names[i]);
					newDown.append(", ");
				}
				else {
					newUp = newUp.append(m_button_names[i]);
					newUp.append(", ");
				}
			}
			//set the sprites
			this->setSprite(EDIT_MOUSE_BUTTON_UP_SPRITE, newUp);
			this->setSprite(EDIT_MOUSE_BUTTON_DOWN_SPRITE, newDown);
			//update the frame counter
			m_frameCounter++;
			return 1;
		}
		//on keyboard
		else if (p_e->getType().compare(df::KEYBOARD_EVENT)==0) {
			df::LogManager& logman = df::LogManager::getInstance();
			//cast it to the correct event type
			const df::EventKeyboard* keyEvent = dynamic_cast<const df::EventKeyboard*> (p_e);
			//if its key pressed
			if (keyEvent->getKeyboardAction() == df::KEY_PRESSED) {
				//convert the enum to a keycode
				unsigned int keycode=((unsigned int) keyEvent->getKey())+1;
				//if the keycode is valid
				if (keycode < m_downKeyButtons.size()) {
					//set the corisponding flag in the keys down vector
					m_downKeyButtons[keycode] = true;
					//recreate the keys down texture
					std::string pressedKeys="down keys: ";
					for (int i = 0; i < m_downKeyButtons.size(); i++) {
						if (m_downKeyButtons[i]) {
							pressedKeys = pressedKeys.append(m_key_names[i]).append(", ");
							logman.writeLog("%s%s", "key pressed: ", m_key_names[i].c_str());
						}
					}
					//set the keys down texture
					this->setSprite(EDIT_KEY_DOWN_SPRITE, pressedKeys);
				}
				return 1;
			}
			//if its key releaased
			else if(keyEvent->getKeyboardAction() == df::KEY_RELEASED){
				//convert the enum to a keycode
				unsigned int keycode = ((unsigned int)keyEvent->getKey()) + 1;
				//if the keycode is valid
				if (keycode < m_downKeyButtons.size()) {
					//set the corisponding flag in the keys down vector
					m_downKeyButtons[keycode] = false;
					//recreate the keys down texture
					std::string pressedKeys = "down keys: ";
					for (int i = 0; i < m_downKeyButtons.size(); i++) {
						if (m_downKeyButtons[i]) {
							pressedKeys = pressedKeys.append(m_key_names[i]).append(", ");
							logman.writeLog("%s%s", "key released: ", m_key_names[i].c_str());
						}
					}
					//set the keys down texture
					this->setSprite(EDIT_KEY_DOWN_SPRITE, pressedKeys);
				}
			}
			return 1;
		}
		//on mouse
		else if (p_e->getType().compare(df::MSE_EVENT) == 0) {
			df::LogManager& logman = df::LogManager::getInstance();
			//cast the event to the correct event type
			const df::EventMouse* mouseEvent = dynamic_cast<const df::EventMouse*>(p_e);
			//if its a click event
			if (mouseEvent->getMouseAction() == df::CLICKED) {
				//reset the correct timer depending on the button
				switch (mouseEvent->getMouseButton()) {
					case df::Mouse::LEFT:
						m_timeSinceLastClicks[0].delta();
						logman.writeLog("%s", "left mouse clicked!");
						break;
					case df::Mouse::MIDDLE:
						m_timeSinceLastClicks[1].delta();
						logman.writeLog("%s", "middle mouse clicked!");
						break;
					case df::Mouse::RIGHT:
						m_timeSinceLastClicks[2].delta();
						logman.writeLog("%s", "right mouse clicked!");
						break;
					default:
						m_timeSinceLastClicks[3].delta();
						logman.writeLog("%s", "undefined mouse clicked!");
						break;
				}
				
			}
			//if its a button down event
			else if (mouseEvent->getMouseAction() == df::BUTTONDOWN) {
				//set the correct falg depending on the button
				switch (mouseEvent->getMouseButton()) {
					case df::Mouse::LEFT:
						m_downMouseButtons[0] = true;
						logman.writeLog("%s", "left mouse down!");
						break;
					case df::Mouse::MIDDLE:
						m_downMouseButtons[1] = true;
						logman.writeLog("%s", "middle mouse down!");
						break;
					case df::Mouse::RIGHT:
						m_downMouseButtons[2] = true;
						logman.writeLog("%s", "right mouse down!");
						break;
					default:
						m_downMouseButtons[3] = true;
						logman.writeLog("%s", "undefined mouse down!");
						break;
				}
				
				
			}
			//if its a button up event
			else if (mouseEvent->getMouseAction() == df::BUTTONUP) {
				//det the correct falg depending on the button
				switch (mouseEvent->getMouseButton()) {
				case df::Mouse::LEFT:
					m_downMouseButtons[0] = false;
					logman.writeLog("%s", "left mouse up!");
					break;
				case df::Mouse::MIDDLE:
					m_downMouseButtons[1] = false;
					logman.writeLog("%s", "middle mouse up!");
					break;
				case df::Mouse::RIGHT:
					m_downMouseButtons[2] = false;
					logman.writeLog("%s", "right mouse up!");
					break;
				default:
					m_downMouseButtons[3] = false;
					logman.writeLog("%s", "undefined mouse up!");
					break;
				}
				
			}
			//if its a movement event
			else if (mouseEvent->getMouseAction() == df::MOVED) {
				//update the current pos vector
				m_mousePos = mouseEvent->getMousePosition();
				//recreate the pos texture 
				std::string newMousePos="mouse pos: x:";
				newMousePos.append(std::to_string(m_mousePos.getX()));
				newMousePos.append(" y: ");
				newMousePos.append(std::to_string(m_mousePos.getY()));
				//set the texture
				this->setSprite(EDIT_MOUSE_MOVE_SPRITE, newMousePos);
				logman.writeLog("%s%s", "mouse moved! ",newMousePos.c_str());
			}
			return 1;
		}

	

		return 0;
	}
	
}