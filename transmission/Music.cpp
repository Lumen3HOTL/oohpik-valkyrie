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

	// Associate music buffer with file.
	// Return 0 if ok, else -1.
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

	// Set label associated with music.
	void Music::setLabel(std::string new_label) {
		m_label = new_label;
	}

	// Get label associated with music.
	std::string Music::getLabel() const {
		return m_label;
	}

	// Play music.
	// If loop is true, repeat play when done.
	void Music::play(bool loop) {
		if (m_music == nullptr) {

			return;
		}
		m_music->setLooping(loop);
		m_music->play();
	}

	// Stop music.
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

	// Return pointer to SFML music.
	sf::Music* Music::getMusic() {
		return m_music;
	}
}