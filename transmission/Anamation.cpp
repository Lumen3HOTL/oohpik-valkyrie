#include "Animation.h"
#include <iostream>
namespace df {
	// Animation c o n s t r u c t o r
	Animation::Animation() {
		m_p_sprite = nullptr;
		
		m_name="undefined_anim"; // S p r i t e name i n ResourceManager .
		m_index=0; // C u r r e n t i n d e x frame f o r S p r i t e .
		m_slowdown_count=0; // Slowdown c o u n t e r .
	}

	// S e t a s s o c i a t e d S p r i t e t o new one .
			// Note , S p r i t e i s managed by ResourceManager .
			// S e t S p r i t e i n d e x t o 0 ( f i r s t frame ) .
	void Animation::setSprite(Sprite* p_new_sprite) {
		m_p_sprite = p_new_sprite;
		m_index = 0;
	}

	// Return p o i n t e r t o a s s o c i a t e d S p r i t e .
	Sprite* Animation::getSprite() const {
		return m_p_sprite;
	}

	// S e t S p r i t e name ( i n ResourceManager ) .
	void Animation::setName(std::string new_name) {
		m_name = new_name;
	}

	// Get S p r i t e name ( i n ResourceManager ) .
	std::string Animation::getName() const {
		return m_name;
	}

	// S e t i n d e x o f c u r r e n t S p r i t e frame t o b e d i s p l a y e d .
	void Animation::setIndex(int new_index) {
		m_index = new_index;
		if (m_p_sprite != NULL) {
			if (m_p_sprite->getFrameCount() < m_index) {
				m_index = m_p_sprite->getFrameCount();
			}
		}
	}

	// Get i n d e x o f c u r r e n t S p r i t e frame t o b e d i s p l a y e d .
	int Animation::getIndex() const {
		return m_index;
	}

	// S e t a n i m a ti o n slowdown c o u n t (−1 means s t o p a n i m a ti o n ) .
	void Animation::setSlowdownCount(int new_slowdown_count) {
		m_slowdown_count = new_slowdown_count;
	}

	// S e t a n i m a ti o n slowdown c o u n t (−1 means s t o p a n i m a ti o n ) .
	int Animation::getSlowdownCount() const {
		return m_slowdown_count;
	}

	// Draw s i n g l e frame c e n t e r e d a t p o s i t i o n ( x , y ) .
	// Drawing a c c o u n t s f o r slowdown , and a d v a n c e s S p r i t e frame .
	// Return 0 i f ok , e l s e −1.
	int Animation::draw(Vector position) {

		//i hate this algorithm it makes no sense and i dont understand it, and its poor separation of concerns but because of how the engine is architected i cant change it. (internal screaming)
		if (m_p_sprite == nullptr) {
			return -1;
		}

		int error = m_p_sprite->draw(m_index, position);





		if (m_slowdown_count == -1) {
			return error;
		}
		m_slowdown_count++;

		if (m_slowdown_count >= m_p_sprite->getSlowdown()) {
			m_slowdown_count = 0;
			m_index++;
			if (m_index >= m_p_sprite->getFrameCount()) {
				m_index = 0;
			}
		}
	
		return error;
	}

	Box Animation::getBox() const {
		if (m_p_sprite == nullptr) {
			return Box(Vector(-0.5, -0.5), 0.99, 0.99);

		}

		return Box(Vector(-1 * (m_p_sprite->getWidth() / 2), -1 * (m_p_sprite->getHeight() / 2)), m_p_sprite->getWidth(), m_p_sprite->getHeight());
	}

}