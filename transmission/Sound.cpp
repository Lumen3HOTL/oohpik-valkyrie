
#include "Sound.h"

namespace df {
	Sound::Sound() {
		m_label = "UndefinedSound";
		m_sound_buffer = new sf::SoundBuffer();
		m_p_sound = nullptr;
	}
	Sound::~Sound() {
		m_label = "";
		
		if (m_p_sound != nullptr) {
			delete m_p_sound;
			m_p_sound = nullptr;
		}
		if (m_sound_buffer != nullptr) {
			delete m_sound_buffer;
			m_sound_buffer = nullptr;
		}
		
	}

	// Load sound b u f f e r from f i l e .
		// Return 0 i f ok , e l s e −1.
	int Sound::loadSound(std::string filename) 
	{
		if (m_sound_buffer != nullptr) {
			delete m_sound_buffer;
			m_sound_buffer = nullptr;
		}
		m_sound_buffer =new sf::SoundBuffer();
		if (!m_sound_buffer->loadFromFile(filename)) {
			return -1;
		}
		m_p_sound = new sf::Sound(*m_sound_buffer);
		return 0;
	}


	// S e t l a b e l a s s o c i a t e d w i t h sound .
	void Sound::setLabel(std::string new_label) {
		m_label = new_label;
	}

	// Get l a b e l a s s o c i a t e d w i t h sound .
	std::string Sound::getLabel() const {
		return m_label;
	}

	// P l a y sound .
	// I f l o o p i s t r u e , r e p e a t p l a y when done .
	void Sound::play(bool loop) {
		if (m_p_sound == nullptr) {
			return;
		}
		m_p_sound->setLooping(loop);
		m_p_sound->play();
	}

	// S to p sound .
	void Sound::stop() {
		if (m_p_sound == nullptr) {
			return;
		}
		m_p_sound->stop();
	}

	// Pause sound .
	void Sound::pause() {
		if (m_p_sound == nullptr) {
			return;
		}
		m_p_sound->pause();
	}

	// Return SFML sound .
	sf::Sound Sound::getSound() const {
		if (m_p_sound == nullptr) {
			sf::SoundBuffer tempbuffer;
			sf::Sound temp=sf::Sound(tempbuffer);
			return temp;
		}
		return *m_p_sound;
	}
}