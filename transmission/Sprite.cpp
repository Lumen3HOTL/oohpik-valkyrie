#include "Sprite.h"
#include "DisplayManager.h"

namespace df {
	Sprite::Sprite() {
		// Sprite always has one arg, the frame count.
		m_color = UNDEFINED_COLOR;
		m_frame=std::vector<Frame>();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
		m_max_frame_count = 0;
		m_slowdown = 0;
		char m_transparency=NULL; // Sprite transparent character (0 if none).
		m_custom_color = 0;
	}


	// Destroy sprite, deleting any allocated frames.
	Sprite::~Sprite() {
		m_color = UNDEFINED_COLOR;
		m_frame.clear();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
		m_max_frame_count = 0;
		m_slowdown = 0;
	}

	// Create sprite with indicated maximum number of frames.
	Sprite::Sprite(int max_frames) {
		m_max_frame_count = max_frames;
		m_color = UNDEFINED_COLOR;
		m_frame = std::vector<Frame>();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
	
		m_slowdown = 0;
		char m_transparency = NULL; // Sprite transparent character (0 if none).
		m_custom_color = 0;
	}
	// Set width of sprite.
	void Sprite::setWidth(int new_width) {
		m_width = new_width;
	}

	// Get width of sprite.
	int Sprite::getWidth() const {
		return m_width;
	}

	// Set height of sprite.
	void Sprite::setHeight(int new_height) {
		m_height = new_height;

	}

	// Get height of sprite.
	int Sprite::getHeight() const {
		return m_height;
	}

	// Set sprite color.
	void Sprite::setColor(Color new_color) {
		m_color = new_color;
	}

	// Get sprite color.
	Color Sprite::getColor() const {
		return m_color;
	}

	// Get total count of frames in sprite.
	int Sprite::getFrameCount() const {
		return m_frame_count;
	}

	// Add frame to sprite.
	// Return -1 if frame array full, else 0.
	int Sprite::addFrame(Frame new_frame) {
		if (m_frame_count >= m_max_frame_count) {
			return -1;
		}
		m_frame.push_back(new_frame);
		m_frame_count++;
		return 0;
	}

	// Get next sprite frame indicated by number.
	// Return empty frame if out of range [0, m_frame_count - 1].
	Frame Sprite::getFrame(int frame_number) const {
		if ((frame_number < 0) || (frame_number > m_frame_count - 1)) {
			return Frame();
		}
		return m_frame[frame_number];
	}

	// Set label associated with sprite.
	void Sprite::setLabel(std::string new_label) {
		m_label = new_label;
	}

	// Get label associated with sprite.
	std::string Sprite::getLabel() const {
		return m_label;
	}

	// Set animation slowdown value.
	// Value in multiples of GameManager frame time.
	void Sprite::setSlowdown(int new_sprite_slowdown) {
		m_slowdown = new_sprite_slowdown;
	}

	// Get animation slowdown value.
	// Value in multiples of GameManager frame time.
	int Sprite::getSlowdown() const {
		return m_slowdown;
	}

	// Draw indicated frame centered at position (x, y).
	// Return 0 if ok, else -1.
	// Note: top-left coordinate is (0, 0).
	int Sprite::draw(int frame_number, Vector position) const {
		if ((frame_number < 0) || (frame_number > m_frame_count - 1)) {
			return -1;
		}
		if (m_color == CUSTOM_COLOR) {
			DisplayManager& dm = DisplayManager::getInstance();
			uint32_t origonalCustom = dm.getCustomColor();
			dm.setCustomColor(m_custom_color);
			int success = m_frame[frame_number].draw(position, m_color,m_transparency);
			dm.setCustomColor(origonalCustom);
			return success;
		}
		int success = m_frame[frame_number].draw(position, m_color,m_transparency);
		return success;
	}

	// Set Sprite transparency character (0 means none).
	void Sprite::setTransparency(char new_transparency) {
		m_transparency = new_transparency;
	}

	// Get Sprite transparency character (0 means none).
	char Sprite::getTransparency() const {
		return m_transparency;
	}

	void Sprite::setCustomColor(uint32_t new_rgba) {
		m_custom_color = new_rgba;
	}

	uint32_t Sprite::getCustomColor() const {
		return m_custom_color;
	}
}