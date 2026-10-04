#include "Frame.h"
#include "DisplayManager.h"
namespace df {
	// Create empty frame.
	Frame::Frame() {
		m_height = 0;
		m_width = 0;
		m_frame_str = "";
	}

	// Create frame of indicated width and height with string.
	Frame::Frame(int new_width, int new_height, std::string frame_str) {
		m_width = new_width;
		m_height = new_height;
		m_frame_str = frame_str;
	}

	// Set width of frame.
	void Frame::setWidth(int new_width) {
		m_width = new_width;
	}

	// Get width of frame.
	int Frame::getWidth() const {
		return m_width;
	}

	// Set height of frame.
	void Frame::setHeight(int new_height) {
		m_height = new_height;
	}

	// Get height of frame.
	int Frame::getHeight() const {
		return m_height;
	}

	// Set frame characters (stored as string).
	void Frame::setString(std::string new_frame_str) {
		m_frame_str = new_frame_str;
	}

	// Get frame characters (stored as string).
	std::string Frame::getString() const {
		return m_frame_str;
	}

	// Draw self, centered at position (x, y) with color.
	// Return 0 if ok, else -1.
	// Note: top-left coordinate is (0, 0).
	int Frame::draw(Vector position, Color color, char transparent) const {
		DisplayManager& dm = DisplayManager::getInstance();
		if (m_frame_str.empty()) {
			return -1;
		}

		int x_offset = this->getWidth() / 2;
		int y_offset = this->getHeight() / 2;

		Vector dpos;
		int counter = 0;
		if ((transparent == NULL)) {
			for (int y = 0; y < m_height; y++) {
				for (int x = 0; x < m_width; x++) {

					
					dpos.setXY(position.getX() + (x - x_offset), position.getY() + (y - y_offset));
					dm.drawCh(dpos, m_frame_str.at(counter), color);
					counter++;

				}
			}
		}
		else {
			for (int y = 0; y < m_height; y++) {
				for (int x = 0; x < m_width; x++) {

					if (m_frame_str[(y * this->getWidth()) + x] != transparent) {
						dpos.setXY(position.getX() + (x - x_offset), position.getY() + (y - y_offset));
						dm.drawCh(dpos, m_frame_str.at(counter), color);
					}
					counter++;
				}
			}
		}
		return 0;
	}
}