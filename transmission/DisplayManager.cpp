#include "DisplayManager.h"
#include <iostream>
#include "CameraManager.h"
#include "Camera.h"
namespace df {


	// Compute c h a r a c t e r h e i g h t i n p i x e l s , b a s e d on window s i z e .
	float charHeight() {
		DisplayManager& dm = DisplayManager::getInstance();
		/*
		if (dm.getVerticalPixels() <= 0) {
			return 0;
		}
		*/
		
		return ((float)dm.getVerticalPixels()) / ((float)dm.getVertical());
	}

	// Compute c h a r a c t e r w i d t h i n p i x e l s , b a s e d on window s i z e .
	float charWidth() {
		DisplayManager& dm = DisplayManager::getInstance();
		/*
		if (dm.getHorizontalPixels() <= 0) {
			return 0;
		}
		*/
		
		return ((float)dm.getHorizontalPixels()) / ((float)dm.getHorizontal());
	}

	// C o n v e r t ASCII s p a c e s ( x , y ) t o window p i x e l s ( x , y ) .
	Vector spacesToPixels(Vector spaces) {
		return Vector((spaces.getX() * charWidth()), (spaces.getY() * charHeight()));
	}

	// C o n v e r t window p i x e l s ( x , y ) t o ASCII s p a c e s ( x , y ) .
	Vector pixelsToSpaces(Vector pixels) {
		float char_width = charWidth();
		float char_height = charHeight();

		/*
		if (char_width <= 0) {
			char_width = 1;
		}
		if (char_height <= 0) {
			char_height = 1;
		}
		*/
		
		return Vector((pixels.getX() / char_width), (pixels.getY() / char_height));
	}



	DisplayManager::DisplayManager() {
		m_font; // Font u s e d f o r ASCII g r a p h i c s .
		m_p_window=nullptr; // P o i n t e r t o SFML window .
		m_window_horizontal_pixels=0; // H o r i z o n t a l p i x e l s i n window .
		m_window_vertical_pixels=0; // V e r t i c a l p i x e l s i n window .
		m_window_horizontal_chars=0; // H o r i z o n t a l ASCII s p a c e s i n window .
		m_window_vertical_chars=0; // V e r t i c a l ASCII s p a c e s i n window .
		m_window_title = "";
		m_font_file_name = "";
		m_window_background_color=sf::Color();
		m_custom_color_object = sf::Color();
		m_custom_window_background_color_Object = sf::Color();
		m_custom_color = 0;
		this->setType("DisplayManager");
		m_apply_camera = true;
	}

	DisplayManager& DisplayManager::getInstance() {
		static DisplayManager visualTerminal = DisplayManager();
		return visualTerminal;
	}

	

	int DisplayManager::startUp() {
		if (m_p_window != nullptr) {
			return 0;
		}
		m_window_horizontal_pixels = WINDOW_HORIZONTAL_PIXELS_DEFAULT;
		m_window_vertical_pixels = WINDOW_VERTICAL_PIXELS_DEFAULT;
		m_window_horizontal_chars = WINDOW_HORIZONTAL_CHARS_DEFAULT;
		m_window_vertical_chars = WINDOW_VERTICAL_CHARS_DEFAULT;
		m_window_title = WINDOW_TITLE_DEFAULT;
		m_font_file_name = FONT_FILE_DEFAULT;
		m_window_background_color = WINDOW_BACKGROUND_COLOR_DEFAULT;
		m_window_background_color_df = BLACK;
		m_custom_window_background_color = RGBAToUInt32ColorConverter(0,0,0,255);
		m_apply_camera = true;

		sf::Vector2u screenSize = sf::Vector2u(m_window_horizontal_pixels, m_window_vertical_pixels);
		sf::VideoMode screenMode = sf::VideoMode(screenSize);
		
		m_p_window = new sf::RenderWindow(screenMode, m_window_title);

		if (!m_p_window) {
			return -1;
		}

		// Turn o f f mouse c u r s o r f o r window .
		m_p_window->setMouseCursorVisible(false);


		


		// S y n c h r o n i z e r e f r e s h r a t e w i t h m o n i to r .
		m_p_window->setVerticalSyncEnabled(true);

		
		if (m_font.openFromFile(m_font_file_name) == false) {
			if (m_p_window->isOpen()) {
				m_p_window->close();
			}
			delete m_p_window;
			m_p_window = nullptr;
			return -1;
		}


		if (Manager::startUp() < 0) {
			if (m_p_window->isOpen()) {
				m_p_window->close();
			}
			
			delete m_p_window;
			m_p_window = nullptr;
			m_font = sf::Font();
			return -1;
		}

		return 0;
	}





	int DisplayManager::setApplyCamera(bool new_camera_applied) {
		if (this->isStarted()) {
			m_apply_camera = new_camera_applied;
			return 0;
		}
		return -1;
	}

	int DisplayManager::getApplyCamera()const {
		if (this->isStarted()) {
			return m_apply_camera;
		}
		return -1;
	}




	void DisplayManager::shutDown() {
		if (m_p_window != nullptr) {
			if (m_p_window->isOpen()) {
				m_p_window->close();
			}
			delete m_p_window;
			m_p_window = nullptr;
		}
		m_font = sf::Font();
		m_window_horizontal_pixels = 0; // H o r i z o n t a l p i x e l s i n window .
		m_window_vertical_pixels = 0; // V e r t i c a l p i x e l s i n window .
		m_window_horizontal_chars = 0; // H o r i z o n t a l ASCII s p a c e s i n window .
		m_window_vertical_chars = 0; // V e r t i c a l ASCII s p a c e s i n window .
		m_window_title = "";
		m_font_file_name = "";
		m_window_background_color = sf::Color();
		Manager::shutDown();
	}


	int DisplayManager::drawCh(Vector world_pos, char ch, Color color) const {
		if (this->isStarted()) {
			Vector world_pos2 = world_pos;

			if (m_apply_camera) {
				CameraManager& cam = CameraManager::getInstance();
				if (!cam.isEmpty()) {
					world_pos2 = world_pos2 - cam.getCurrentCamera()->getCameraPos();
				}
				
			}

			if ((world_pos2.getX() >= 0) && (world_pos2.getX() <= this->getHorizontal()) && (world_pos2.getY() >= 0) && (world_pos2.getY() <= this->getVertical())) {
				Vector pixelPos = spacesToPixels(world_pos2);

				sf::Vector2f maskSize = sf::Vector2f(charWidth(), charHeight());
				sf::Vector2f maskPos = sf::Vector2f((pixelPos.getX() - (charWidth() / 10)), (pixelPos.getY() - (charHeight() / 5)));
				sf::RectangleShape matteMask;

				matteMask.setSize(maskSize);
				matteMask.setPosition(maskPos);
				matteMask.setFillColor(m_window_background_color);

				m_p_window->draw(matteMask);

				sf::Text textToDraw(m_font);
				textToDraw.setStyle(sf::Text::Bold); // S e t t e x t s t y l e .
				textToDraw.setString(ch);

				if (charWidth() < charHeight()) {
					textToDraw.setCharacterSize(charWidth() * 2);
				}
				else {
					textToDraw.setCharacterSize(charHeight() * 2);
				}

				sf::Color textColor;
				switch (color) {
				case BLACK:
					textColor = sf::Color::Black;
					break;
				case RED:
					textColor = sf::Color::Red;
					break;
				case GREEN:
					textColor = sf::Color::Green;
					break;
				case YELLOW:
					textColor = sf::Color::Yellow;
					break;
				case BLUE:
					textColor = sf::Color::Blue;
					break;
				case PURPLE:
					textColor = COLOR_OBJECT_PURPLE;
					break;
				case ORANGE:
					textColor = COLOR_OBJECT_ORANGE;
					break;
				case BROWN:
					textColor = COLOR_OBJECT_BROWN;
					break;
				case MAGENTA:
					textColor = sf::Color::Magenta;
					break;
				case CYAN:
					textColor = sf::Color::Cyan;
					break;
				case WHITE:
					textColor = sf::Color::White;
					break;
				case CUSTOM_COLOR:
					textColor = m_custom_color_object;
					break;
				default:
					textColor = ERROR_COLOR;
					break;
				}

				sf::Vector2f textPos = sf::Vector2f(pixelPos.getX(), pixelPos.getY());

				textToDraw.setPosition(textPos);
				textToDraw.setFillColor(textColor);

				m_p_window->draw(textToDraw);
			}
			
			return 0;
		}
		return -1;
	}


	int DisplayManager::drawString(Vector pos, std::string str, Justification just, Color color, HeightAllignment allignment) const {
		Vector startPos = Vector(pos);
		
		int startX = 0;
		std::vector<float> startingXs;
		float startY = 0;
		std::vector<std::string> lines;
		std::string current;
		for (int c = 0; c < str.size(); c++) {
			if (str.at(c) == '\n') {
				lines.push_back(current);
				current = "";
				if (c == str.size() - 1) {
					lines.push_back(current);
				}
			}
			else {
				current.push_back(str.at(c));
			}
		}
		if (current.size() != 0) {
			
			lines.push_back(current);
		}
		
		switch (just) {
			case CENTER_JUSTIFIED:
				for (int l = 0; l < lines.size(); l++) {
					startingXs.push_back(startPos.getX() - (lines[l].size() / 2));
				}
				break;
			case RIGHT_JUSTIFIED:
				for (int l = 0; l < lines.size(); l++) {
					startingXs.push_back(startPos.getX() - lines[l].size());
				}
				
				break;
			default:
				for (int l = 0; l < lines.size(); l++) {
					startingXs.push_back(startPos.getX());
				}
				break;
		}

		switch (allignment) {
			case CENTER_ALLIGNED:
				startY = (startPos.getY()) - lines.size() / 2;
				break;
			case BOTTOM_ALLIGNED:
				startY = startPos.getY() - lines.size();
				break;
			default:
				startY = startPos.getY();
				break;
		}

		Vector currentPos = Vector(startPos.getX(),startY);
		int error = 0;
		for (int l = 0; l < lines.size(); l++) {
			currentPos.setX(startingXs[l]);
			if (lines[l].empty()) {
				currentPos.setY(currentPos.getY() + 1);
			}
			else {
				for (int c = 0; c < lines[l].size(); c++) {


					error = this->drawCh(currentPos, lines[l].at(c), color);

					currentPos.setX(currentPos.getX() + 1);
				}
				currentPos.setY(currentPos.getY() + 1);
			}
			
		}
		
		return error;
	}

	int DisplayManager::getHorizontal() const{
		if (this->isStarted()) {
			return m_window_horizontal_chars;
		}
		return -1;
	}

	int DisplayManager::getVertical() const {
		if (this->isStarted()) {
			return m_window_vertical_chars;
		}
		return -1;
	}

	// Return window ’ s h o r i z o n t a l maximum ( i n p i x e l s ) .
	int DisplayManager::getHorizontalPixels() const {
		if (this->isStarted()) {
			return m_window_horizontal_pixels;
		}
		return -1;
	}

	// Return window ’ s v e r t i c a l maximum ( i n p i x e l s ) .
	int DisplayManager::getVerticalPixels() const {
		if (this->isStarted()) {
			return m_window_vertical_pixels;
		}
		return -1;
	}

	int DisplayManager::swapBuffers() {
		if (this->isStarted()) {
			m_p_window->display();
			m_p_window->clear(m_window_background_color);
			return 0;
		}
		return -1;
	}

	int DisplayManager::clear() const {
		if (this->isStarted()) {
			m_p_window->clear(m_window_background_color);
			return 0;
		}
		return -1;
	}

	sf::RenderWindow* DisplayManager::getWindow() const {
		if (this->isStarted()) {
			return m_p_window;
		}
		return nullptr;
	}

	bool DisplayManager::setCustomColor(const std::uint32_t newCustom) {
		if (this->isStarted()) {
			m_custom_color = newCustom;
			m_custom_color_object = sf::Color::Color(newCustom);
			return true;
		}
		return false;
	}

	std::uint32_t DisplayManager::getCustomColor() const {
		if (this->isStarted()) {
			return m_custom_color;
		}
		return 0;
	}


	bool DisplayManager::setBackgroundColor(Color new_color) {
		if (this->isStarted()) {
			m_window_background_color_df=new_color;
			sf::Color backgroundColor;
			switch (new_color) {
			case BLACK:
				backgroundColor = sf::Color::Black;
				break;
			case RED:
				backgroundColor = sf::Color::Red;
				break;
			case GREEN:
				backgroundColor = sf::Color::Green;
				break;
			case YELLOW:
				backgroundColor = sf::Color::Yellow;
				break;
			case BLUE:
				backgroundColor = sf::Color::Blue;
				break;
			case PURPLE:
				backgroundColor = COLOR_OBJECT_PURPLE;
				break;
			case ORANGE:
				backgroundColor = COLOR_OBJECT_ORANGE;
				break;
			case BROWN:
				backgroundColor = COLOR_OBJECT_BROWN;
				break;
			case MAGENTA:
				backgroundColor = sf::Color::Magenta;
				break;
			case CYAN:
				backgroundColor = sf::Color::Cyan;
				break;
			case WHITE:
				backgroundColor = sf::Color::White;
				break;
			case CUSTOM_COLOR:
				backgroundColor = m_custom_window_background_color_Object;
				break;
			default:
				backgroundColor = ERROR_COLOR;
				break;
			}
			m_window_background_color = backgroundColor;
			this->clear();
			return true;
		}
		return false;
	}

	Color DisplayManager::getBackgroundColor() const {
		if (this->isStarted()) {
			return m_window_background_color_df;
		}
		return UNDEFINED_COLOR;
	}

	std::uint32_t DisplayManager::getCustomWindowBackgroundColor() const {
		if (this->isStarted()) {
			return m_custom_window_background_color;
		}
		return 0;
	}

	bool DisplayManager::setCustomBackgroundColor(const std::uint32_t newCustom) {
		if (this->isStarted()) {
			m_custom_window_background_color = newCustom;
			m_custom_window_background_color_Object = sf::Color::Color(m_custom_window_background_color);
			this->clear();
			return true;
		}
		return false;
	}
}