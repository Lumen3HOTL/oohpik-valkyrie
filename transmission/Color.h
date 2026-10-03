#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
namespace df {
	// C o l o r s D r a g o n f l y r e c o g n i z e s .
	enum Color {
		UNDEFINED_COLOR = -1,
		BLACK = 0,
		RED,
		GREEN,
		YELLOW,
		ORANGE,
		BROWN,
		BLUE,
		PURPLE,
		MAGENTA,
		CYAN,
		WHITE,
		CUSTOM_COLOR,
	};
	enum channelToExtract {
		CHANNEL_R,
		CHANNEL_G,
		CHANNEL_B,
		CHANNEL_A,
	};
	
	std::uint8_t extractInt32ColorChannel(const uint32_t rgbaColor, const channelToExtract channel);
	std::uint32_t RGBAToUInt32ColorConverter(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
	// I f c o l o r n o t s p e c i f i e d , w i l l u s e t h i s .
	const Color COLOR_DEFAULT = WHITE;
	const sf::Color COLOR_OBJECT_BROWN = sf::Color::Color(0x875f00ff);
	const sf::Color ERROR_COLOR = sf::Color::Color(0x9d692eff);
	const sf::Color COLOR_OBJECT_ORANGE = sf::Color::Color(0xff9e00ff);
	const sf::Color COLOR_OBJECT_PURPLE = sf::Color::Color(0x800080ff);
}
