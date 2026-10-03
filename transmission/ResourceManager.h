#pragma once
#include <string>
#include <vector>
#include "Manager.h"
#include "Sprite.h"
#include "Sound.h"
#include "Music.h"
namespace df {
	const int MAX_SPRITES = 500;
	const int MAX_SOUNDS = 50;
	const int MAX_MUSICS = 50;
	class ResourceManager : public Manager{
	
		private:
			ResourceManager(); // P r i v a t e ( a s i n g l e t o n ) .
			ResourceManager(ResourceManager const&); // Don ’ t a l l o w copy .
			void operator =(ResourceManager const&); // Don ’ t a l l o w a s s i g n m e n t .
			std::vector<Sprite*> m_p_sprite; // Array o f s p r i t e s .
			int m_sprite_count; // Count o f number o f l o a d e d s p r i t e s .
			std::string cleanLine(std::string toClean);
			std::string prepareNumericLine(std::string toClean);
			uint64_t parseNumericLineULong(std::string toParse);
			int parseNumericLineInt(std::string toParse);

			std::vector<Sound*> m_sound; // Array o f sound b u f f e r s .
			int m_sound_count; // Count o f number o f l o a d e d s o u n d s .
			std::vector<Music*> m_music; // Array o f music b u f f e r s .
			int m_music_count; // Count o f number o f l o a d e d m u s i c s .


			
			const std::array<char,10> m_validArabicNumerics={'1','2','3','4','5','6','7','8','9','0'};
			const std::array<char,16> m_validHexNumerics={ '1', '2', '3', '4', '5', '6', '7', '8', '9','0','a','b','c','d','e','f' };
			
			const std::array<std::string,12> m_validColors = { "black","red","green","yellow","orange","brown","blue","purple","magenta","cyan","white","custom_color"};
		public:
			// Get t h e one and o n l y i n s t a n c e o f t h e ResourceManager .
			static ResourceManager& getInstance();
			// Get ResourceManager r e a d y t o manager f o r r e s o u r c e s .
			int startUp();
			
			// S h u t down ResourceManager , f r e e i n g up any a l l o c a t e d S p r i t e s .
			void shutDown();
			
			// Load S p r i t e from f i l e .
			// A s s i g n i n d i c a t e d l a b e l t o s p r i t e .
			// Return 0 i f ok , e l s e −1.
			int loadSprite(std::string filename, std::string label);
			
			// Unload S p r i t e w i t h i n d i c a t e d l a b e l .
			// Return 0 i f ok , e l s e −1.
			int unloadSprite(std::string label);
			
			// Find S p r i t e w i t h i n d i c a t e d l a b e l .
			// Return p o i n t e r t o i t i f found , e l s e NULL.
			Sprite * getSprite(std::string label) const;






			// Load Sound from f i l e .
			// Return 0 i f ok , e l s e −1.
			int loadSound(std::string filename, std::string label);
			
			// Remove Sound w i t h i n d i c a t e d l a b e l .
			// Return 0 i f ok , e l s e −1.
			int unloadSound(std::string label);
			
			// Find Sound w i t h i n d i c a t e d l a b e l .
			// Return p o i n t e r t o i t i f found , e l s e NULL.
			Sound * getSound(std::string label);
			
			// A s s o c i a t e f i l e w i t h Music .
			// Return 0 i f ok , e l s e −1.
			int loadMusic(std::string filename, std::string label);
			
			// Remove l a b e l f o r Music w i t h i n d i c a t e d l a b e l .
			// Return 0 i f ok , e l s e −1.
			int unloadMusic(std::string label);
			
			// Find Music w i t h i n d i c a t e d l a b e l .
			// Return p o i n t e r t o i t i f found , e l s e NULL.
			Music * getMusic(std::string label);
			
	};
}