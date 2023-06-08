#include "pch.h"
#include "image_manip.h"

namespace {
	bmp_image create_bmp_image(int32_t width, int32_t height, bool has_alpha) {
		bmp_image image;

		if (width <= 0 || height <= 0) {
			// values cannot be 0 or negative
			return image;
		}

		image.info_header.width = width;
		image.info_header.height = height;

		if (has_alpha) {
			image.info_header.size = sizeof(bmp_info_header) + sizeof(bmp_color_header);
			image.file_header.offset_data = sizeof(bmp_file_header) + sizeof(bmp_info_header) + sizeof(bmp_color_header);
			image.info_header.bit_count = 32;
			image.info_header.compression = 3;
			image.row_stride = width * 4;
			image.data.resize(image.row_stride * height);
			image.file_header.file_size = image.file_header.offset_data + image.data.size();
		}
		else {
			image.info_header.size = sizeof(bmp_info_header);
			image.file_header.offset_data = sizeof(bmp_file_header) + sizeof(bmp_info_header);

			image.info_header.bit_count = 24;
			image.info_header.compression = 0;
			image.row_stride = width * 3;
			image.data.resize(image.row_stride * height);

			uint32_t temp_stride = image.row_stride;
			while (temp_stride % 4 != 0) {
				temp_stride++;
			}

			uint32_t new_stride = temp_stride;
			image.file_header.file_size = image.file_header.offset_data + image.data.size() + image.info_header.height * (new_stride - image.row_stride);
		}

		return image;
	}

	void set_pixels(bmp_image& image, uint32_t x_position, uint32_t y_position, uint32_t width, uint32_t height, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
		if (x_position + width > (uint32_t)image.info_header.width ||
			y_position + height > (uint32_t)image.info_header.height) {
			// The region does not fit in the image
			return;
		}

		uint32_t channels = image.info_header.bit_count / 8;
		for (uint32_t i = y_position; i < y_position + height; ++i) {
			for (uint32_t j = x_position; j < x_position + width; ++j) {
				image.data[channels * (i * image.info_header.width + j) + 0] = blue;
				image.data[channels * (i * image.info_header.width + j) + 1] = green;
				image.data[channels * (i * image.info_header.width + j) + 2] = red;
				if (channels == 4) {
					image.data[channels * (i * image.info_header.width + j) + 3] = alpha;
				}
			}
		}
	}
}