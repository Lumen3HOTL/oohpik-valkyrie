#include "ResourceManager.h"
#include "LogManager.h"
#include <fstream>
#include <stdexcept>

namespace df {
	ResourceManager::ResourceManager() {
		m_sprite_count = 0;
		m_p_sprite = std::vector<Sprite*>();
		this->setType("ResourceManager");
		m_sound= std::vector<Sound*>(); // Array of sound buffers.
		m_sound_count=0; // Count of number of loaded sounds.
		 m_music= std::vector<Music*>(); // Array of music buffers.
		m_music_count=0; // Count of number of loaded musics.
	}
	// Get the one and only instance of the ResourceManager.
	ResourceManager& ResourceManager::getInstance() {
		static ResourceManager resourceMan = ResourceManager();
		return resourceMan;
	}
	// Get ResourceManager ready to manager for resources.
	int ResourceManager::startUp() {
		if (this->isStarted()) {
			return -1;
		}
		m_sprite_count = 0;
		m_p_sprite.clear();

		return Manager::startUp();
	}

	// Shut down ResourceManager, freeing up any allocated Sprites.
	void ResourceManager::shutDown() {
		if (!m_p_sprite.empty()) {
			for (int sprite = 0; sprite < m_sprite_count; sprite++) {
				delete m_p_sprite[sprite];
			}
		}
		if (!m_music.empty()) {
			for (int m = 0; m < m_music_count; m++) {
				delete m_music[m];

			}
		}
		if (!m_sound.empty()) {
			for (int s = 0; s < m_music_count; s++) {
				delete m_sound[s];

			}
		}
		m_music.clear();
		m_sound.clear();
		m_sprite_count = 0;
		m_music_count = 0;
		m_sound_count = 0;
		m_p_sprite.clear();
		Manager::shutDown();
	}
	std::string ResourceManager::cleanLine(std::string toClean) {
		if (this->isStarted()) {
			//the cleaned copy
			std::string textLine = "";
			//if we have started to read out the actual data yet
			bool mode = false;
			unsigned char currentChar = 0; 
			//simple state machine
			for (int i = 0; i < toClean.length(); i++) {
				//if we have found the start of the data
				if (mode) {
					currentChar = static_cast<unsigned char>(toClean.at(i));
					//if the current character is not a space
					if ((!std::isspace(currentChar))) {
						//copy it out
						textLine.push_back(tolower(currentChar));
					}
					//otherwise we have found the end of the data and should stop
					else {
						break;
					}
				}
				//if we havent found the start of the data yet
				else {
					//if the current character is not a space
					currentChar = static_cast<unsigned char>(toClean.at(i));
					if ((!std::isspace(currentChar))) {
						//copy it out and mark that we have found the start
						textLine.push_back(tolower(currentChar));
						mode = true;
					}
				}

			}

			return textLine;
		}
		return "";
	}
	std::string ResourceManager::prepareNumericLine(std::string toClean) {
		if (this->isStarted()) {
			//remove any starting and ending spaces
			std::string toClean2 = this->cleanLine(toClean);


			//the cleaned text to convert to an int
			std::string textLine = "";
			//if this is in hex or not
			bool mode = false;
			//the character to start reading out from
			int start = 0;
			//if we found a valid character at this index
			bool charFound = false;
			//determine if we have a hex string on one of the two common encodings
			if (toClean2.size() >= 2) {
				if ((toClean2.at(0) == '#')) {
					mode = true;
					start = 1;
				}
			}
			if (toClean2.size() >= 3) {
				if (((toClean2.at(0) == '0') && (toClean2.at(1) == 'x'))) {
					mode = true;
					start = 2;
				}
			}
			//if hex mode
			if (mode) {
				textLine.push_back('0');
				textLine.push_back('x');
				for (int i = start; i < toClean2.length(); i++) {
					charFound = false;
					for (int test = 0; test < m_validHexNumerics.size(); test++) {
						if (toClean2.at(i) == m_validHexNumerics.at(test)) {
							textLine.push_back(toClean2.at(i));
							charFound = true;
						}

					}
					if (!charFound) {
						return "";
					}

				}
			}
			//if arabic mode
			else {
				for (int i = start; i < toClean2.length(); i++) {
					charFound = false;
					for (int test = 0; test < m_validArabicNumerics.size(); test++) {
						if (toClean2.at(i) == m_validArabicNumerics.at(test)) {
							textLine.push_back(toClean2.at(i));
							charFound = true;
						}

					}
					if (!charFound) {
						return "";
					}

				}
			}

			return textLine;
		}
		return "";
	}
	uint64_t ResourceManager::parseNumericLineULong(std::string toParse) {
		std::string preparedString= this->prepareNumericLine(toParse);
		if (preparedString.empty()) {
			return UINT64_MAX;
		}
		size_t finalSize = 0;

		uint64_t rv = UINT64_MAX;
		
		
		try {
			rv=std::stoul(std::string(preparedString), &finalSize,0);
		}
		catch (std::invalid_argument) {

			return UINT64_MAX;
		}
		catch (std::out_of_range) {

			return UINT64_MAX;
		}

		if (finalSize != preparedString.size()) {
			return UINT64_MAX;
		}
		return rv;
	}

	int ResourceManager::parseNumericLineInt(std::string toParse) {
		
		std::string preparedString = this->prepareNumericLine(toParse);
		if (preparedString.empty()) {
			return -1;
		}
		int rv = -1;
		try {
			rv = std::stoi(std::string(preparedString));
		}
		catch (std::invalid_argument) {

			return -1;
		}
		catch (std::out_of_range) {

			return -1;
		}
		return rv;
	}
	// Load Sprite from file.
	// Assign indicated label to sprite.
	// Return 0 if ok, else -1.
	int ResourceManager::loadSprite(std::string filename, std::string label) {

		if (this->isStarted()) {
			// log manager and log message init stuff
			LogManager& lm = LogManager::getInstance();
			std::string logMessage = "loading: ";
			logMessage = logMessage.append(label).append(" from: ").append(filename).append("\n");
			//safety dance
			if (m_sprite_count >= MAX_SPRITES) {
				logMessage = logMessage.append("load error: simaltainous sprite limit reached!");
				lm.writeLog(logMessage.c_str());

				return -1;
			}
			//if the sprite list isnt empty, check for duplicates before starting the really expensive loading process
			if (!m_p_sprite.empty()) {
				for (int spriteindex = 0; spriteindex < m_p_sprite.size(); spriteindex++) {
					if (m_p_sprite[spriteindex]->getLabel().compare(label) == 0) {
						logMessage = logMessage.append("load error: sprite with the label: \"").append(label).append("\" already exists!");
						lm.writeLog(logMessage.c_str());
						return -1;
					}
				}
			}
			
			//the file
			std::ifstream spriteFile(filename);
			
			//just a variable pair used for logging
			int currentline = 0;
			bool isNonFatalError = false;

			//init the values to sentinals
			int frames=-1;
			int width = -1;
			int height = -1;
			int slowdown = -1;
			
			Color color = UNDEFINED_COLOR;
			uint32_t customColor = 0;
		
			//did the file open successfully check
			if (!spriteFile.is_open()) {
				logMessage = logMessage.append("load error: asset file could not be loaded!");
				lm.writeLog(logMessage.c_str());
				return -1;
			}
			
			//define the string we will reuse everywhere
			std::string inputLine;
			//get the next line
			//loop until we find the first line
			while (true) {
				
				//load the next line and is the file not done yet check(if so close it and log and error out)
				if (!getline(spriteFile, inputLine)) {
					logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
					lm.writeLog(logMessage.c_str());
					spriteFile.close();
					return -1;
				}
				
				//if the line is not empty, start reading in the data process
				if (!cleanLine(inputLine).empty()) {
					break;
				}
				//if its empty, log it but continue
				else {
					logMessage = logMessage.append("load error: data not started yet at line: ").append(std::to_string(currentline)).append("!\n");
					isNonFatalError = true;
				}
				currentline++;
			}
			
			//parse the line with the helper functions
			frames = this->parseNumericLineInt(inputLine);
			//if its invalid, safely error out
			if (frames < 1) {
				logMessage = logMessage.append("load error: invalid frame count of: ").append(std::to_string(frames)).append(" at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			
			//update the line number
			currentline++;
			
			
			//load the next line and is the file not done yet check(if so close it and log and error out)
			if (!getline(spriteFile, inputLine)) {
				logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			
			//parse the line with the helper functions
			width = this->parseNumericLineInt(inputLine);
			//if its invalid, safely error out
			if (width < 1) {
				logMessage = logMessage.append("load error: invalid width of: ").append(std::to_string(width)).append(" at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			
			
			//update the line number
			currentline++;
			//load the next line and is the file not done yet check(if so close it and log and error out)
			if (!getline(spriteFile, inputLine)) {
				logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			//parse the line with the helper functions
			height = this->parseNumericLineInt(inputLine);
			//if its invalid, safely error out
			if (height < 1) {
				logMessage = logMessage.append("load error: invalid height of: ").append(std::to_string(height)).append(" at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}

			currentline++;
			
			
			//load the next line and is the file not done yet check(if so close it and log and error out)
			if (!getline(spriteFile, inputLine)) {
				logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			//parse the line with the helper functions
			slowdown = this->parseNumericLineInt(inputLine);
			//if its invalid, safely error out
			if (slowdown < 0) {
				logMessage = logMessage.append("load error: invalid slowdown of: ").append(std::to_string(slowdown)).append(" at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			
			
			
			currentline++;
			//load the next line and is the file not done yet check(if so close it and log and error out)
			if (!getline(spriteFile, inputLine)) {
				logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			//parse the line with the helper functions
			inputLine = this->cleanLine(inputLine);
			//variable to detect if we need to parse one extra config line
			bool parseCustom = false;
			//messy but more or less efficent color handling
			for (int c = 0; c < m_validColors.size(); c++) {
				if (inputLine.compare(m_validColors[c]) == 0) {
					color = (Color)(c);
					if (c == m_validColors.size() - 1) {
						parseCustom = true;
					}
					break;
				}
			}
			//log that no valid color was found, but continue
			if (color == UNDEFINED_COLOR) {
				logMessage = logMessage.append("load error: no valid color found at line: ").append(std::to_string(currentline)).append("! invalid value: ").append(inputLine).append("\n");
				isNonFatalError = true;
			}

			//if a custom color is detected, read it in
			if (parseCustom) {
				//update the line number
				currentline++;
				//load the next line and is the file not done yet check(if so close it and log and error out)
				if (!getline(spriteFile, inputLine)) {
					logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
					lm.writeLog(logMessage.c_str());
					spriteFile.close();
					return -1;
				}
				//parse the line with the handly helepr functions
				uint64_t maybeAColor = this->parseNumericLineULong(inputLine);
				//if its invalid, safely error out
				if (maybeAColor == UINT64_MAX) {
					logMessage = logMessage.append("load error: invalid custom color value of: ").append(std::to_string(maybeAColor)).append(" at line: ").append(std::to_string(currentline)).append("!");
					lm.writeLog(logMessage.c_str());
					spriteFile.close();
					return -1;
				}
				//set the custom color
				customColor=(uint32_t)maybeAColor;
			}
			//update the line number
			bool transparencyDefined = false;
			bool first = false;
			currentline++;
			if (!getline(spriteFile, inputLine)) {
				logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
				lm.writeLog(logMessage.c_str());
				spriteFile.close();
				return -1;
			}
			if (cleanLine(inputLine).compare("define_transparency_char") == 0) {
				transparencyDefined = true;
			}
			char transparency = NULL;
			
			
			if (transparencyDefined) {
				currentline++;
				if (!getline(spriteFile, inputLine)) {
					logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
					lm.writeLog(logMessage.c_str());
					spriteFile.close();
					return -1;
				}
				if (inputLine.length() <= 0) {
					logMessage = logMessage.append("load error: no valid transparency character found at line: ").append(std::to_string(currentline)).append("!");
					lm.writeLog(logMessage.c_str());
					spriteFile.close();
					return -1;
				}
				else if (inputLine.length() > 1) {
					logMessage = logMessage.append("load error: line too long at length: ").append(std::to_string(inputLine.size())).append(" at line: ").append(std::to_string(currentline)).append("!\n");
					isNonFatalError = true;
				}
				transparency = inputLine.at(0);
				
			}
			else {
				first = true;
			}
			//create the sprite
			Sprite* newSprite = new Sprite(frames);
			//give ti the attribvutes we just decoded
			newSprite->setWidth(width);
			newSprite->setHeight(height);
			newSprite->setSlowdown(slowdown);
			newSprite->setColor(color);
			newSprite->setCustomColor(customColor);
			newSprite->setLabel(label);
			if (transparencyDefined) {
				newSprite->setTransparency(transparency);
			}
			//create a frame and texture string on the stack, to save allocations later
			Frame nextFrame;
			std::string nextTexture;
			//finally read in the frames
			for (int frame = 0; frame < frames; frame++) {
				//reset the frame and textrue objects
				nextFrame = Frame();
				nextTexture = "";
				//for the height of frame
				for (int h = 0; h < height; h++) {
					//update the current line number
					currentline++;
					if (!first) {
						//load the next line and is the file not done yet check(if so close it and delete the in progress sprite and log and error out)
						if (!getline(spriteFile, inputLine)) {
							logMessage = logMessage.append("load error: file ended early at line: ").append(std::to_string(currentline)).append("!");
							lm.writeLog(logMessage.c_str());
							spriteFile.close();
							delete newSprite;
							return -1;
						}
					}
					first = false;
					
					//if the line isnt long enough  close the file and delete the in progress sprite and log and error out
					if (inputLine.size() < width) {
						logMessage = logMessage.append("load error: line too short at length: ").append(std::to_string(inputLine.size())).append(" at line: ").append(std::to_string(currentline)).append("!");
						lm.writeLog(logMessage.c_str());
						spriteFile.close();
						delete newSprite;
						return -1;
					}
					//if the line is too long, log the error but dont do anything
					else if (inputLine.size() > width) {
						logMessage = logMessage.append("load error: line too long at length: ").append(std::to_string(inputLine.size())).append(" at line: ").append(std::to_string(currentline)).append("!\n");
						isNonFatalError = true;
					}
					for (int w = 0; w < width; w++) {
						nextTexture.push_back(inputLine.at(w));
					}
					
				}
				//give the frame the attributes we just extracted
				nextFrame.setString(nextTexture);
				nextFrame.setHeight(height);
				nextFrame.setWidth(width);
				//put the frame in the sprite
				newSprite->addFrame(nextFrame);
			}
			//save the sprite and update the sprite count
			m_p_sprite.push_back(newSprite);
			m_sprite_count++;
			
			//detect extra data after the defined area
			int extraLines = 0;
			while (true) {
				if (getline(spriteFile, inputLine)) {
					extraLines++;
					
				}
				else {
					break;
				}
				
					
			}
			//finally clsoe the file, we dont need it anymore
			spriteFile.close();
			//if we have extra data after the defined area, log it but dont do anything
			if (extraLines > 0) {
				logMessage = logMessage.append("load error: more lines than neccesary, extra lines: ").append(std::to_string(extraLines)).append(" at line: ").append(std::to_string(currentline)).append("!");
				isNonFatalError = true;
			}

			
			if (isNonFatalError) {
				logMessage = logMessage.append("load completed with errors!");
				
			}
			else {
				logMessage = logMessage.append("successful!");

			}
			lm.writeLog(logMessage.c_str());
			
			return 0;
		}
			
		
		return -1;
	}

	// Unload Sprite with indicated label.
	// Return 0 if ok, else -1.
	int ResourceManager::unloadSprite(std::string label) {
		if (this->isStarted()) {
			int found = -1;
			
			for (int sprite = m_sprite_count-1; sprite >=0 ; sprite--) {
				if (m_p_sprite[sprite]->getLabel().compare(label)==0) {
					delete m_p_sprite[sprite];
					m_p_sprite[sprite] = m_p_sprite[m_sprite_count - 1];
					m_sprite_count--;
					m_p_sprite.pop_back();
					found = 0;
				}
			}
			
				

			
			return found;
		}
		return -1;
	}

	// Find Sprite with indicated label.
	// Return pointer to it if found, else NULL.
	Sprite* ResourceManager::getSprite(std::string label) const {
		if (this->isStarted()) {
			for (int sprite = 0; sprite < m_sprite_count; sprite++) {
				if (m_p_sprite[sprite]->getLabel().compare(label) == 0) {
					return m_p_sprite[sprite];
				}
			}
			
		}
		return nullptr;
	}




	// Load Sound from file.
	// Return 0 if ok, else -1.
	int  ResourceManager::loadSound(std::string filename, std::string label) {
		if (m_sound_count >= MAX_SOUNDS) {
			return -1;
		}
		for (int i = 0; i < m_sound_count; i++) {
			if (m_sound[i]->getLabel().compare(label) == 0) {
				return -1;
			}
		}
		Sound* soundTemp = new Sound();

		if (soundTemp->loadSound(filename) == -1) {
			delete soundTemp;
			return -1;
		}

		soundTemp->setLabel(label);
		m_sound.push_back(soundTemp);
		m_sound_count++;
		return 0;
	}

	// Remove Sound with indicated label.
	// Return 0 if ok, else -1.
	int  ResourceManager::unloadSound(std::string label) {
		if (m_sound_count > 0) {
			int found = -1;
			for (int s = m_sound_count - 1; s >= 0; s--) {
				if (m_sound[s]->getLabel().compare(label) == 0) {
					delete m_sound[s];
					m_sound[s] = m_sound[m_sound_count - 1];
					m_sound_count--;
					m_sound.pop_back();
					found = 0;
				}
			}
			return found;
		}
		return -1;
	}

	// Find Sound with indicated label.
	// Return pointer to it if found, else NULL.
	Sound* ResourceManager::getSound(std::string label) {
		for (int s = 0; s < m_sound_count; s++) {
			if (m_sound[s]->getLabel().compare(label) == 0) {
				return m_sound[s];
			}
		}
		return nullptr;
	}

	// Associate file with Music.
	// Return 0 if ok, else -1.
	int  ResourceManager::loadMusic(std::string filename, std::string label) {
		if (m_music_count >= MAX_MUSICS) {
			return -1;
		}
		
		for (int i = 0; i < m_music_count; i++) {
			if (m_music[i]->getLabel().compare(label) == 0) {
				return -1;
			}
		}
		Music* musicTemp = new Music();

		if (musicTemp->loadMusic(filename) == -1) {
			delete musicTemp;
			return -1;
		}

		musicTemp->setLabel(label);
		m_music.push_back(musicTemp);
		m_music_count++;
		return 0;
	}

	// Remove label for Music with indicated label.
	// Return 0 if ok, else -1.
	int  ResourceManager::unloadMusic(std::string label) {
		if (m_music_count > 0) {
			int found = -1;
			for (int m = m_music_count - 1; m >= 0; m--) {
				if (m_music[m]->getLabel().compare(label) == 0) {
					delete m_music[m];
					m_music[m] = m_music[m_music_count - 1];
					m_music_count--;
					m_music.pop_back();
					found = 0;
				}
			}
			return found;
		}
		return -1;
	}

	// Find Music with indicated label.
	// Return pointer to it if found, else NULL.
	Music* ResourceManager::getMusic(std::string label) {
		for (int m = 0; m < m_music_count; m++) {
			if (m_music[m]->getLabel().compare(label) == 0) {
				return m_music[m];
			}
		}
		return nullptr;
	}
}