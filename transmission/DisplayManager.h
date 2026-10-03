#pragma once
// System i n c l u d e s .
#include <SFML/Graphics.hpp>
#include <string>
// Engine i n c l u d e s .
#include  "Color.h"
#include "Manager.h"
#include "Vector.h"
#include <cstdint>

namespace df {
	// D e f a u l t s f o r SFML window .
	const int WINDOW_HORIZONTAL_PIXELS_DEFAULT = 1024;
	const int WINDOW_VERTICAL_PIXELS_DEFAULT = 768;
	const int WINDOW_HORIZONTAL_CHARS_DEFAULT = 80;
	const int WINDOW_VERTICAL_CHARS_DEFAULT = 24;
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
	// Compute c h a r a c t e r h e i g h t i n p i x e l s , b a s e d on window s i z e .
	float charHeight();
	
	// Compute c h a r a c t e r w i d t h i n p i x e l s , b a s e d on window s i z e .
	float charWidth();
	
	// C o n v e r t ASCII s p a c e s ( x , y ) t o window p i x e l s ( x , y ) .
	Vector spacesToPixels(Vector spaces);
	
	// C o n v e r t window p i x e l s ( x , y ) t o ASCII s p a c e s ( x , y ) .
	Vector pixelsToSpaces(Vector pixels);


	class DisplayManager : public Manager{
		
		private:
		DisplayManager(); // P r i v a t e ( a s i n g l e t o n ) .
		DisplayManager(DisplayManager const&); // Don ’ t a l l o w copy .
		void operator =(DisplayManager const&); // Don ’ t a l l o w a s s i g n m e n t
		sf::Font m_font; // Font u s e d f o r ASCII g r a p h i c s .
		sf::RenderWindow * m_p_window; // P o i n t e r t o SFML window .
		int m_window_horizontal_pixels; // H o r i z o n t a l p i x e l s i n window .
		int m_window_vertical_pixels; // V e r t i c a l p i x e l s i n window .
		int m_window_horizontal_chars; // H o r i z o n t a l ASCII s p a c e s i n window .
		int m_window_vertical_chars; // V e r t i c a l ASCII s p a c e s i n window .
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
		// Get t h e one and o n l y i n s t a n c e o f t h e D i s p l a y M a n a g e r .
			static DisplayManager& getInstance();
		
			// Open g r a p h i c s window , r e a d y f o r t e x t −b a s e d d i s p l a y .
			// Return 0 i f ok , e l s e −1.
			int startUp();
		
			// C l o s e g r a p h i c s window .
			void shutDown();
		
			// Draw c h a r a c t e r a t window l o c a t i o n ( x , y ) w i t h c o l o r .
			// Return 0 i f ok , e l s e −1.
			int drawCh(Vector world_pos, char ch, Color color) const;
			// Draw string a t window l o c a t i o n ( x , y ) w i t h c o l o r .
			// Return 0 i f ok , e l s e −1.
			int drawString(Vector pos, std::string str, Justification just, Color color, HeightAllignment allignment=TOP_ALLIGNED) const;
		
			// Return window ’ s h o r i z o n t a l maximum ( i n c h a r a c t e r s ) .
			int getHorizontal() const;
		
			// Return window ’ s v e r t i c a l maximum ( i n c h a r a c t e r s ) .
			int getVertical() const;
		
			// Return window ’ s h o r i z o n t a l maximum ( i n p i x e l s ) .
			int getHorizontalPixels() const;
		
			// Return window ’ s v e r t i c a l maximum ( i n p i x e l s ) .
			int getVerticalPixels() const;
		
			// Render c u r r e n t window b u f f e r .
			// Return 0 i f ok , e l s e −1.
			int swapBuffers();
			
			std::uint32_t getCustomColor() const;

			bool setCustomColor(const std::uint32_t newCustom);
			// Return p o i n t e r t o SFML g r a p h i c s window .
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