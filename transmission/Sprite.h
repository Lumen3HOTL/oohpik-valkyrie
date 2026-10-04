#pragma once
#include "Frame.h"
#include <vector>
#include <string>
#include "Clock.h"
namespace df {
	class Sprite {

		private:
			int m_width; // Sprite width.
			int m_height; // Sprite height.
			int m_max_frame_count; // Max number frames sprite can have.
			int m_frame_count; // Actual number frames sprite has.
			Color m_color; // Optional color for entire sprite.
			int m_slowdown; // Animation slowdown (1=no slowdown, 0=stop).
			std::vector<Frame> m_frame; // Array of frames.
			std::string m_label; // Text label to identify sprite.
			char m_transparency; // Sprite transparent character (0 if none).
			Sprite(); // Sprite always has one arg, the frame count.
			uint32_t m_custom_color;

		public:
			// Destroy sprite, deleting any allocated frames.
			~Sprite();

			// Create sprite with indicated maximum number of frames.
			Sprite(int max_frames);
			// Set width of sprite.
			void setWidth(int new_width);

			// Get width of sprite.
			int getWidth() const;

			// Set height of sprite.
			void setHeight(int new_height);

			// Get height of sprite.
			int getHeight() const;

			// Set sprite color.
			void setColor(Color new_color);

			// Get sprite color.
			Color getColor() const;

			// Get total count of frames in sprite.
			int getFrameCount() const;

			// Add frame to sprite.
			// Return -1 if frame array full, else 0.
			int addFrame(Frame new_frame);

			// Get next sprite frame indicated by number.
			// Return empty frame if out of range [0, m_frame_count - 1].
			Frame getFrame(int frame_number) const;

			// Set label associated with sprite.
			void setLabel(std::string new_label);

			// Get label associated with sprite.
			std::string getLabel() const;

			// Set animation slowdown value.
			// Value in multiples of GameManager frame time.
			void setSlowdown(int new_sprite_slowdown);

			// Get animation slowdown value.
			// Value in multiples of GameManager frame time.
			int getSlowdown() const;

			// Draw indicated frame centered at position (x, y).
			// Return 0 if ok, else -1.
			// Note: top-left coordinate is (0, 0).
			int draw(int frame_number, Vector position) const;

			void setCustomColor(uint32_t new_rgba);

			uint32_t getCustomColor() const;

			// Set Sprite transparency character (0 means none).
			void setTransparency(char new_transparency);
			
			// Get Sprite transparency character (0 means none).
			char getTransparency() const;

	};
}

