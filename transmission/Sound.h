#pragma once


// System i n c l u d e s .
#include <string>
#include <SFML/Audio.hpp>
namespace df {
	class Sound {
		
		private:
			sf::Sound* m_p_sound; // The SFML sound .
			sf::SoundBuffer* m_sound_buffer; // SFML sound b u f f e r a s s o c i a t e d w i t h sound .
			std::string m_label; // Text l a b e l t o i d e n t i f y sound .
		
		public:
		Sound();
		~Sound();
		
		// Load sound b u f f e r from f i l e .
		// Return 0 i f ok , e l s e −1.
		int loadSound(std::string filename);
		
		// S e t l a b e l a s s o c i a t e d w i t h sound .
		void setLabel(std::string new_label);
		
		// Get l a b e l a s s o c i a t e d w i t h sound .
		std::string getLabel() const;
		
		// P l a y sound .
		// I f l o o p i s t r u e , r e p e a t p l a y when done .
		void play(bool loop = false);
		
		// S to p sound .
		void stop();
		
		// Pause sound .
		void pause();
		
		// Return SFML sound .
		sf::Sound getSound() const;
		
	};
}
