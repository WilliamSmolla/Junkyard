#pragma once
#include <inttypes.h>

struct color {
	color() = default;
	color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
		: red{ red }, green{ green }, blue{ blue }, alpha{ alpha } {}

	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	uint8_t alpha = 0;
};

	



