#include "Animation.h"
#include <iostream>
namespace df {
	// Animation constructor
	Animation::Animation() {
		m_p_sprite = nullptr;
		
		m_name="undefined_anim"; // Sprite name in ResourceManager.
		m_index=0; // Current index frame for Sprite.
		m_slowdown_count=0; // Slowdown counter.
	}

	// Set associated Sprite to new one.
			// Note, Sprite is managed by ResourceManager.
			// Set Sprite index to 0 (first frame).
	void Animation::setSprite(Sprite* p_new_sprite) {
		m_p_sprite = p_new_sprite;
		m_index = 0;
	}

	// Return pointer to associated Sprite.
	Sprite* Animation::getSprite() const {
		return m_p_sprite;
	}

	// Set Sprite name (in ResourceManager).
	void Animation::setName(std::string new_name) {
		m_name = new_name;
	}

	// Get Sprite name (in ResourceManager).
	std::string Animation::getName() const {
		return m_name;
	}

	// Set index of current Sprite frame to be displayed.
	void Animation::setIndex(int new_index) {
		m_index = new_index;
		if (m_p_sprite != NULL) {
			if (m_p_sprite->getFrameCount() < m_index) {
				m_index = m_p_sprite->getFrameCount();
			}
		}
	}

	// Get index of current Sprite frame to be displayed.
	int Animation::getIndex() const {
		return m_index;
	}

	// Set animation slowdown count (-1 means stop animation).
	void Animation::setSlowdownCount(int new_slowdown_count) {
		m_slowdown_count = new_slowdown_count;
	}

	// Set animation slowdown count (-1 means stop animation).
	int Animation::getSlowdownCount() const {
		return m_slowdown_count;
	}

	// Draw single frame centered at position (x, y).
	// Drawing accounts for slowdown, and advances Sprite frame.
	// Return 0 if ok, else -1.
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