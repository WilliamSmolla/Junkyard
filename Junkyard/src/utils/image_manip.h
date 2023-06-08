#pragma once
#include "image_types.h"
#include "graphics/colors.h"

// BMP image
namespace {
	static bmp_image create_bmp_image(int32_t width, int32_t height, bool has_alpha = true);

	static void set_pixel(bmp_image& image, uint32_t x_position, uint32_t y_position, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
	static void set_pixel(bmp_image& image, uint32_t x_position, uint32_t y_position, color color);
	static void set_pixels(bmp_image& image, uint32_t x_position, uint32_t y_position, uint32_t width, uint32_t height, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
	static void set_pixels(bmp_image& image, uint32_t x_position, uint32_t y_position, uint32_t width, uint32_t height, color color);
}
