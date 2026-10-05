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
			ResourceManager(); // Private (a singleton).
			ResourceManager(ResourceManager const&); // Don't allow copy.
			void operator =(ResourceManager const&); // Don't allow assignment.
			std::vector<Sprite*> m_p_sprite; // Array of sprites.
			int m_sprite_count; // Count of number of loaded sprites.
			std::string cleanLine(std::string toClean);
			std::string prepareNumericLine(std::string toClean);
			uint64_t parseNumericLineULong(std::string toParse);
			int parseNumericLineInt(std::string toParse);

			std::vector<Sound*> m_sound; // Array of sound buffers.
			int m_sound_count; // Count of number of loaded sounds.
			std::vector<Music*> m_music; // Array of music buffers.
			int m_music_count; // Count of number of loaded musics.


			
			const std::array<char,10> m_validArabicNumerics={'1','2','3','4','5','6','7','8','9','0'};
			const std::array<char,16> m_validHexNumerics={ '1', '2', '3', '4', '5', '6', '7', '8', '9','0','a','b','c','d','e','f' };
			
			const std::array<std::string,12> m_validColors = { "black","red","green","yellow","orange","brown","blue","purple","magenta","cyan","white","custom_color"};
		public:
			// Get the one and only instance of the ResourceManager.
			static ResourceManager& getInstance();
			// Get ResourceManager ready to manager for resources.
			int startUp();
			
			// Shut down ResourceManager, freeing up any allocated Sprites.
			void shutDown();
			
			// Load Sprite from file.
			// Assign indicated label to sprite.
			// Return 0 if ok, else -1.
			int loadSprite(std::string filename, std::string label);
			
			// Unload Sprite with indicated label.
			// Return 0 if ok, else -1.
			int unloadSprite(std::string label);
			
			// Find Sprite with indicated label.
			// Return pointer to it if found, else NULL.
			Sprite * getSprite(std::string label) const;






			// Load Sound from file.
			// Return 0 if ok, else -1.
			int loadSound(std::string filename, std::string label);
			
			// Remove Sound with indicated label.
			// Return 0 if ok, else -1.
			int unloadSound(std::string label);
			
			// Find Sound with indicated label.
			// Return pointer to it if found, else NULL.
			Sound * getSound(std::string label);
			
			// Associate file with Music.
			// Return 0 if ok, else -1.
			int loadMusic(std::string filename, std::string label);
			
			// Remove label for Music with indicated label.
			// Return 0 if ok, else -1.
			int unloadMusic(std::string label);
			
			// Find Music with indicated label.
			// Return pointer to it if found, else NULL.
			Music * getMusic(std::string label);
			
	};
}

#define RM df::ResourceManager::getInstance()