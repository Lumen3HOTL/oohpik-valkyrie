#include "Color.h"

namespace df {
	std::uint8_t extractInt32ColorChannel(const uint32_t rgbaColor, const channelToExtract channel) {
		switch (channel) {
		case CHANNEL_R:
			return (uint8_t)((rgbaColor >> 24) & ((uint32_t)(255)));
		case CHANNEL_G:
			return (uint8_t)((rgbaColor >> 16) & ((uint32_t)(255)));
		case CHANNEL_B:
			return (uint8_t)((rgbaColor >> 8) & ((uint32_t)(255)));
		case CHANNEL_A:
			return (uint8_t)((rgbaColor) & ((uint32_t)(255)));
		default:
			return (uint8_t)((rgbaColor) & ((uint32_t)(255)));
		}

	}
	std::uint32_t RGBAToUInt32ColorConverter(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
		std::uint32_t bigR = (std::uint32_t)r;
		std::uint32_t bigG = (std::uint32_t)g;
		std::uint32_t bigB = (std::uint32_t)b;
		std::uint32_t bigA = (std::uint32_t)a;
		return (std::uint32_t)((bigR << 24) | (bigG << 16) | (bigB << 8) | bigA);
	}
}