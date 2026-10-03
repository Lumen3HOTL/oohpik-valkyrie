#include "Frame.h"
#include "DisplayManager.h"
namespace df {
	// C r e a t e empty frame .
	Frame::Frame() {
		m_height = 0;
		m_width = 0;
		m_frame_str = "";
	}

	// C r e a t e frame o f i n d i c a t e d w i d t h and h e i g h t w i t h s t r i n g .
	Frame::Frame(int new_width, int new_height, std::string frame_str) {
		m_width = new_width;
		m_height = new_height;
		m_frame_str = frame_str;
	}

	// S e t w i d t h o f frame .
	void Frame::setWidth(int new_width) {
		m_width = new_width;
	}

	// Get w i d t h o f frame .
	int Frame::getWidth() const {
		return m_width;
	}

	// S e t h e i g h t o f frame .
	void Frame::setHeight(int new_height) {
		m_height = new_height;
	}

	// Get h e i g h t o f frame .
	int Frame::getHeight() const {
		return m_height;
	}

	// S e t frame c h a r a c t e r s ( s t o r e d a s s t r i n g ) .
	void Frame::setString(std::string new_frame_str) {
		m_frame_str = new_frame_str;
	}

	// Get frame c h a r a c t e r s ( s t o r e d a s s t r i n g ) .
	std::string Frame::getString() const {
		return m_frame_str;
	}

	// Draw s e l f , c e n t e r e d a t p o s i t i o n ( x , y ) w i t h c o l o r .
	// Return 0 i f ok , e l s e −1.
	// Note : top − l e f t c o o r d i n a t e i s ( 0 , 0 ) .
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