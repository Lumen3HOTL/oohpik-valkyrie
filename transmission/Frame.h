#pragma once
#include <string>
#include "Vector.h"
#include "Color.h"
namespace df {

	
	class Frame {
	private:
		int m_width; // Width o f frame .
		int m_height; // Height of frame.
		std::string m_frame_str; // All frame characters stored as string.
	
	public:
		// Create empty frame.
		Frame();
		
		// Create frame of indicated width and height with string.
		Frame(int new_width, int new_height, std::string frame_str);
		
		// Set width of frame.
		void setWidth(int new_width);
		
		// Get width of frame.
		int getWidth() const;
	
		// Set height of frame.
		void setHeight(int new_height);
	
		// Get height of frame.
		int getHeight() const;
		
		// Set frame characters (stored as string).
		void setString(std::string new_frame_str);
		
		// Get frame characters (stored as string).
		std::string getString() const;
		
		// Draw self, centered at position (x, y) with color.
		// Return 0 if ok, else -1.
		// Note: top-left coordinate is (0, 0).
		int draw(Vector position, Color color, char transparent = NULL) const;
	};
}