#include "Music.h"

namespace df {
	Music::Music() {
		m_label = "undefinedMusic";
		m_music = nullptr;
	}
	Music::~Music() {
		m_label = "";
		if (m_music != nullptr) {
			delete m_music;
		}
		
		m_music = nullptr;
	}

	// A s s o c i a t e music b u f f e r w i t h f i l e .
	// Return 0 i f ok , e l s e −1.
	int Music::loadMusic(std::string filename) {
		if (m_music != nullptr) {
			delete m_music;
			
		}
		
		m_music = new sf::Music();
		if (!m_music->openFromFile(filename)) {
			return -1;
		}
		return 0;
	}

	// S e t l a b e l a s s o c i a t e d w i t h music .
	void Music::setLabel(std::string new_label) {
		m_label = new_label;
	}

	// Get l a b e l a s s o c i a t e d w i t h music .
	std::string Music::getLabel() const {
		return m_label;
	}

	// P l a y music .
	// I f l o o p i s t r u e , r e p e a t p l a y when done .
	void Music::play(bool loop) {
		if (m_music == nullptr) {

			return;
		}
		m_music->setLooping(loop);
		m_music->play();
	}

	// S to p music .
	void Music::stop() {
		if (m_music == nullptr) {

			return;
		}
		m_music->stop();
	}

	// Pause music .
	void Music::pause() {
		if (m_music == nullptr) {

			return;
		}
		m_music->pause();
	}

	// Return p o i n t e r t o SFML music .
	sf::Music* Music::getMusic() {
		return m_music;
	}
}