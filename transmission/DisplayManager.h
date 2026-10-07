#pragma once
// System includes.
#include <SFML/Graphics.hpp>
#include <string>
// Engine includes.
#include  "Color.h"
#include "Manager.h"
#include "Vector.h"
#include <cstdint>

namespace df {
	// Defaults for SFML window.
	const int WINDOW_HORIZONTAL_PIXELS_DEFAULT = 1472; // 115 chars * 12.8 px, same cell size as the original 1024/80
	const int WINDOW_VERTICAL_PIXELS_DEFAULT = 960; // 30 chars * 32 px, same cell size as the original 768/24
	const int WINDOW_HORIZONTAL_CHARS_DEFAULT = 115; // matches the original ookpik map width
	const int WINDOW_VERTICAL_CHARS_DEFAULT = 30; // matches the original ookpik map height
	const int WINDOW_STYLE_DEFAULT = sf::Style::Titlebar;
	const sf::Color WINDOW_BACKGROUND_COLOR_DEFAULT = sf::Color::Black;
	const std::string WINDOW_TITLE_DEFAULT = "dragonfly";
	const std::string FONT_FILE_DEFAULT = "df-font.ttf";

	

	enum Justification {
		LEFT_JUSTIFIED,
		CENTER_JUSTIFIED,
		RIGHT_JUSTIFIED,

	};
	enum HeightAllignment {
		TOP_ALLIGNED,
		CENTER_ALLIGNED,
		BOTTOM_ALLIGNED,

	};
	// Compute character height in pixels, based on window size.
	float charHeight();
	
	// Compute character width in pixels, based on window size.
	float charWidth();
	
	// Convert ASCII spaces (x, y) to window pixels (x, y).
	Vector spacesToPixels(Vector spaces);
	
	// Convert window pixels (x, y) to ASCII spaces (x, y).
	Vector pixelsToSpaces(Vector pixels);


	class DisplayManager : public Manager{
		
		private:
		DisplayManager(); // Private (a singleton).
		DisplayManager(DisplayManager const&); // Don't allow copy.
		void operator =(DisplayManager const&); // Don't allow assignment
		sf::Font m_font; // Font used for ASCII graphics.
		sf::RenderWindow * m_p_window; // Pointer to SFML window.
		int m_window_horizontal_pixels; // Horizontal pixels in window.
		int m_window_vertical_pixels; // Vertical pixels in window.
		int m_window_horizontal_chars; // Horizontal ASCII spaces in window.
		int m_window_vertical_chars; // Vertical ASCII spaces in window.
		sf::Color m_window_background_color;
		std::string m_window_title;
		std::string m_font_file_name;
		std::uint32_t m_custom_color;
		sf::Color m_custom_color_object;
		std::uint32_t m_custom_window_background_color;
		sf::Color m_custom_window_background_color_Object;
		Color m_window_background_color_df;
		bool m_apply_camera;

	public:
		// Get the one and only instance of the DisplayManager.
			static DisplayManager& getInstance();
		
			// Open graphics window, ready for text-based display.
			// Return 0 if ok, else -1.
			int startUp();
		
			// Close graphics window.
			void shutDown();
		
			// Draw character at window location (x, y) with color.
			// Return 0 if ok, else -1.
			int drawCh(Vector world_pos, char ch, Color color) const;
			// Draw string at window location (x, y) with color.
			// Return 0 if ok, else -1.
			int drawString(Vector pos, std::string str, Justification just, Color color, HeightAllignment allignment=TOP_ALLIGNED) const;
		
			// Return window's horizontal maximum (in characters).
			int getHorizontal() const;
		
			// Return window's vertical maximum (in characters).
			int getVertical() const;
		
			// Return window's horizontal maximum (in pixels).
			int getHorizontalPixels() const;
		
			// Return window's vertical maximum (in pixels).
			int getVerticalPixels() const;
		
			// Render current window buffer.
			// Return 0 if ok, else -1.
			int swapBuffers();
			
			std::uint32_t getCustomColor() const;

			bool setCustomColor(const std::uint32_t newCustom);
			// Return pointer to SFML graphics window.
			sf::RenderWindow * getWindow() const;
		
			bool setBackgroundColor(const Color new_color);

			bool setCustomBackgroundColor(const std::uint32_t new_color);

			Color getBackgroundColor() const;

			std::uint32_t getCustomWindowBackgroundColor() const;

			int clear() const;

			int setApplyCamera(bool camera_applied = true);

			int getApplyCamera()const;
	};
}

#define DM df::ResourceManager::getInstance()