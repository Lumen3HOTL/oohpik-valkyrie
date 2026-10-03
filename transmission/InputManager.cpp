#include "InputManager.h"
#include "DisplayManager.h"
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "GameManager.h"

namespace df {




	InputManager::InputManager() {
		this->setType("InputManager");

		m_rightMouseButtonFrameCounter = Clock();
		m_leftMouseButtonFrameCounter = Clock();
		m_middleMouseButtonFrameCounter = Clock();
		m_undefinedMouseButtonFrameCounter = Clock();
		m_leftMouseButtonActive = false;
		m_middleMouseButtonActive = false;
		m_rightMouseButtonActive = false;
		m_undefinedMouseButtonActive = false;
		m_mouseClickTollerance = 0;
	}

	InputManager& InputManager::getInstance() {
		static InputManager inputKeeper = InputManager();
		return inputKeeper;
	}


	int InputManager::startUp() {
		if (this->isStarted()) {
			return -1;
		}
		DisplayManager& display = DisplayManager::getInstance();
		if (!display.isStarted()) {
			return -1;
		}
		
		display.getWindow()->setKeyRepeatEnabled(false);
	
		m_rightMouseButtonFrameCounter.delta();
		m_leftMouseButtonFrameCounter.delta();
		m_middleMouseButtonFrameCounter.delta();
		m_undefinedMouseButtonFrameCounter.delta();
		m_leftMouseButtonActive = false;
		m_middleMouseButtonActive = false;
		m_rightMouseButtonActive = false;
		m_undefinedMouseButtonActive = false;
		m_mouseClickTollerance = (MOUSE_CLICK_TIME+1) * 33;
		Manager::startUp();
		return 0;

	}


	void InputManager::shutDown() {

		m_rightMouseButtonFrameCounter.delta();
		m_leftMouseButtonFrameCounter.delta();
		m_middleMouseButtonFrameCounter.delta();
		m_undefinedMouseButtonFrameCounter.delta();
		m_leftMouseButtonActive = false;
		m_middleMouseButtonActive = false;
		m_rightMouseButtonActive = false;
		m_undefinedMouseButtonActive = false;
		m_mouseClickTollerance = 0;
		Manager::shutDown();

	}

	void InputManager::getInput() {
		//safety check
		if (this->isStarted()) {
			//get the window so we ca npoll it 
			DisplayManager& display = DisplayManager::getInstance();
			sf::RenderWindow* p_window = display.getWindow();
			//prealloc the event to save cycles
			std::optional<sf::Event> p_event;
			//handle the click event timeouts for the different buttons
			if ((m_leftMouseButtonFrameCounter.split() / 1000) > m_mouseClickTollerance) {
				m_leftMouseButtonActive = false;
			}
			if ((m_middleMouseButtonFrameCounter.split() / 1000) > m_mouseClickTollerance) {
				m_middleMouseButtonActive = false;
			}
			if ((m_rightMouseButtonFrameCounter.split() / 1000) > m_mouseClickTollerance) {
				m_rightMouseButtonActive = false;
			}
			if ((m_undefinedMouseButtonFrameCounter.split() / 1000) > m_mouseClickTollerance) {
				m_undefinedMouseButtonActive = false;
			}


			while (p_event = p_window->pollEvent()) {
				
				
				if (p_event->is <sf::Event::Closed>()) {
					GameManager::getInstance().shutDown();
					break;
				}
				//handle keydown
				else if(p_event->is<sf::Event::KeyPressed>()) {
					//get the event
					sf::Event::KeyPressed* sfKeyEvent = p_event->getIf<sf::Event::KeyPressed>();
					//make our own event
					EventKeyboard keyDown = EventKeyboard();
					//set the correct mode
					keyDown.setKeyboardAction(KEY_PRESSED);

					//use this handle ultra fast function i spent way too long on to convert the keycodes, then set the key code with the result
					keyDown.setKey(SFKeycodeToDragonflyKeycode(sfKeyEvent->code));
					//broadcast the event
					this->onEvent(&keyDown);

				}
				//handle key up
				else if (p_event->is<sf::Event::KeyReleased>()) {
					//get the event
					sf::Event::KeyReleased* sfKeyEvent = p_event->getIf<sf::Event::KeyReleased>();
					//make our own version
					EventKeyboard keyUp = EventKeyboard();
					//set the correct mode
					keyUp.setKeyboardAction(KEY_RELEASED);
					//use this handle ultra fast function i spent way too long on to convert the keycodes, then set the key code with the result
					keyUp.setKey(SFKeycodeToDragonflyKeycode(sfKeyEvent->code));
					//boradcast the event
					this->onEvent(&keyUp);

				}
				//handle mouse move
				else if (p_event->is<sf::Event::MouseMoved>()) {
					//get the event
					sf::Event::MouseMoved* sfMouseMoveEvent = p_event->getIf<sf::Event::MouseMoved>();
					//make our own version
					EventMouse mouseMove = EventMouse();
					//set the correct mode
					mouseMove.setMouseAction(MOVED);
					//extract and set the position, adjusting for char space and pixel space coordinates
					mouseMove.setMousePosition(pixelsToSpaces(Vector(sfMouseMoveEvent->position.x, sfMouseMoveEvent->position.y)));

					this->onEvent(&mouseMove);

				}
				//handle mouse down
				else if (p_event->is<sf::Event::MouseButtonPressed>()) {
					//get the event
					sf::Event::MouseButtonPressed* sfMousePress = p_event->getIf<sf::Event::MouseButtonPressed>();
					//make our own version 
					EventMouse mousePress = EventMouse();
					//set the correct mode
					mousePress.setMouseAction(BUTTONDOWN);
					//choose the correct branch based on the button pressed
					switch (sfMousePress->button) {
						case sf::Mouse::Button::Left:
							//activate the click release timeout for this button
							m_leftMouseButtonActive = true;
							//reset the timeout for this button 
							m_leftMouseButtonFrameCounter.delta();
							//set the correct mouse button
							mousePress.setMouseButton(Mouse::LEFT);
							break;

						case sf::Mouse::Button::Middle:
							//acivate the click release timeout for this button
							m_middleMouseButtonActive = true;
							//reset the timeout for this button
							m_middleMouseButtonFrameCounter.delta();
							//set the correct mosue button
							mousePress.setMouseButton(Mouse::MIDDLE);
							break;

						case sf::Mouse::Button::Right:
							//acivate the click release timeout for this button
							m_rightMouseButtonActive = true;
							//reset the timeout for this button
							m_rightMouseButtonFrameCounter.delta();
							//set the correct mosue button
							mousePress.setMouseButton(Mouse::RIGHT);
							break;

						default:
							//acivate the click release timeout for this button
							m_undefinedMouseButtonActive = true;
							//reset the timeout for this button
							m_undefinedMouseButtonFrameCounter.delta();
							//set the correct mouse button
							mousePress.setMouseButton(Mouse::UNDEFINED_MOUSE_BUTTON);
					}
					this->onEvent(&mousePress);
				}
				//handle mouse up
				else if (p_event->is<sf::Event::MouseButtonReleased>()) {
					//get the event
					sf::Event::MouseButtonReleased* sfMouseRelase = p_event->getIf<sf::Event::MouseButtonReleased>();
					//make our own version
					EventMouse mouseRelease = EventMouse();
					//set the correct mode
					mouseRelease.setMouseAction(BUTTONUP);
					//choose the correct branch based on the button input
					switch (sfMouseRelase->button) {
						case sf::Mouse::Button::Left:
							//if the button is released within its click time window
							if (m_leftMouseButtonActive && ((m_leftMouseButtonFrameCounter.split() / 1000) < m_mouseClickTollerance)) {
								//create and send the apropriate click event
								EventMouse mouseClick = EventMouse();
								mouseClick.setMouseAction(CLICKED);
								mouseClick.setMouseButton(Mouse::LEFT);
								this->onEvent(&mouseClick);
							}
							

							//deactivate the click release timout for this button
							m_leftMouseButtonActive = false;
							//set the correct mouse button
							mouseRelease.setMouseButton(Mouse::LEFT);
							break;

						case sf::Mouse::Button::Middle:
							//if the button is released within its click time window
							if (m_middleMouseButtonActive && ((m_middleMouseButtonFrameCounter.split() / 1000) < m_mouseClickTollerance)) {
								//create and send the apropriate click event
								EventMouse mouseClick = EventMouse();
								mouseClick.setMouseAction(CLICKED);
								mouseClick.setMouseButton(Mouse::MIDDLE);
								this->onEvent(&mouseClick);
							}
							//deactivate the click release timout for this button
							m_middleMouseButtonActive = false;
							//set the correct mouse button
							mouseRelease.setMouseButton(Mouse::MIDDLE);
							break;

						case sf::Mouse::Button::Right:
							//if the button is released within its click time window
							if (m_rightMouseButtonActive && ((m_rightMouseButtonFrameCounter.split() / 1000) < m_mouseClickTollerance)) {
								//create and send the apropriate click event
								EventMouse mouseClick = EventMouse();
								mouseClick.setMouseAction(CLICKED);
								mouseClick.setMouseButton(Mouse::RIGHT);
								this->onEvent(&mouseClick);
							}
							//deactivate the click release timout for this button
							m_rightMouseButtonActive = false;
							//set the correct mouse button
							mouseRelease.setMouseButton(Mouse::RIGHT);
							break;

						default:
							//if the button is released within its click time window
							if (m_undefinedMouseButtonActive && ((m_undefinedMouseButtonFrameCounter.split() / 1000) < m_mouseClickTollerance)) {
								//create and send the apropriate click event
								EventMouse mouseClick = EventMouse();
								mouseClick.setMouseAction(CLICKED);
								mouseClick.setMouseButton(Mouse::UNDEFINED_MOUSE_BUTTON);
								this->onEvent(&mouseClick);
							}
							//deactivate the click release timout for this button
							m_undefinedMouseButtonActive = false;
							//set the correct mouse button
							mouseRelease.setMouseButton(Mouse::UNDEFINED_MOUSE_BUTTON);
					}

					this->onEvent(&mouseRelease);

				}
				else if (p_event->is<sf::Event::FocusLost>()) {
					m_leftMouseButtonActive = false;
					m_middleMouseButtonActive = false;
					m_rightMouseButtonActive = false;
					m_undefinedMouseButtonActive = false;
				}

			}
				
		}
	}



}