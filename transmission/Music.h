#pragma once
#include <string>
#include <SFML/Audio.hpp>
namespace df {
	// System i n c l u d e s .
	
	
	
	class Music {

		private:
			Music(Music const&); // SFML d o e s n ’ t a l l o w music copy .
			void operator =(Music const&); // SFML d o e s n ’ t a l l o w music a s s i g n m e n t .
			sf::Music* m_music; // The SFML music .
			std::string m_label; // Text l a b e l t o i d e n t i f y music .

		public:
			Music();
			~Music();

			// A s s o c i a t e music b u f f e r w i t h f i l e .
			// Return 0 i f ok , e l s e −1.
			int loadMusic(std::string filename);

			// S e t l a b e l a s s o c i a t e d w i t h music .
			void setLabel(std::string new_label);

			// Get l a b e l a s s o c i a t e d w i t h music .
			std::string getLabel() const;

			// P l a y music .
			// I f l o o p i s t r u e , r e p e a t p l a y when done .
			void play(bool loop = true);

			// S to p music .
			void stop();

			// Pause music .
			void pause();

			// Return p o i n t e r t o SFML music .
			sf::Music* getMusic();
		};
}