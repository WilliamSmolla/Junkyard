#include "pch.h"
#include "image_io.h"

namespace {
	bmp_image load_bmp_image(std::string filepath) {
		bmp_image image;
		std::ifstream input_stream{ filepath.c_str(), std::ios_base::binary };

		if (input_stream) {
			input_stream.read((char*)&image.file_header, sizeof(bmp_file_header));
			if (image.file_header.file_type != 0x4D42) {
				// not a bmp image;
				// TODO: handle return value properly.
				bmp_image result;
				return result;
			}

			input_stream.read((char*)&image.info_header, sizeof(bmp_info_header));

			if (image.info_header.bit_count == 32) {
				if (image.info_header.size >= (sizeof(bmp_info_header) + sizeof(bmp_color_header))) {
					input_stream.read((char*)&image.color_header, sizeof(bmp_color_header));

					bmp_color_header expected_color_header;
					if (expected_color_header.red_mask != image.color_header.red_mask ||
						expected_color_header.blue_mask != image.color_header.blue_mask ||
						expected_color_header.green_mask != image.color_header.green_mask ||
						expected_color_header.alpha_mask != image.color_header.alpha_mask) {
						// TODO: handle return value properly
						// Unexpected color mask format! The program expects the pixel data to be in the BGRA format
					}
					if (expected_color_header.color_space_type != image.color_header.color_space_type) {
						// TODO: handle return value properly
						// Unexpected color space type. The program expects sRGB values
					}
				}
				else {
					// does not seem to contain bit mask information. Unrecognized file format.
					// TODO: handle return value properly.
				}

				input_stream.seekg(image.file_header.offset_data, input_stream.beg);

				if (image.info_header.bit_count == 32) {
					image.info_header.size = sizeof(bmp_info_header) + sizeof(bmp_color_header);
					image.file_header.offset_data = sizeof(bmp_file_header) + sizeof(bmp_info_header) + sizeof(bmp_color_header);
				}
				else {
					image.info_header.size = sizeof(bmp_info_header);
					image.file_header.offset_data = sizeof(bmp_file_header) + sizeof(bmp_info_header);
				}

				image.file_header.file_size = image.file_header.offset_data;

				if (image.info_header.height < 0) {
					// The program can treat only BMP images with the origin in the bottom left corner
					// TODO: handle return value properly
				}

				image.data.resize(image.info_header.width * image.info_header.height * image.info_header.bit_count);

				if (image.info_header.width % 4 == 0) {
					input_stream.read((char*)image.data.data(), image.data.size());
					image.file_header.file_size += image.data.size();
				}
				else {
					image.row_stride = image.info_header.width * image.info_header.bit_count / 8;

					uint32_t temp_stride = image.row_stride;
					while (temp_stride % 4 != 0) {
						temp_stride++;
					}

					uint32_t new_stride = temp_stride;
					std::vector<uint8_t> padding_row(new_stride - image.row_stride);

					for (int i = 0; i < image.info_header.height; ++i) {
						input_stream.read((char*)(image.data.data() + image.row_stride * i), image.row_stride);
						input_stream.read((char*)padding_row.data(), padding_row.size());
					}
					image.file_header.file_size += image.data.size() + image.info_header.height * padding_row.size();
				}
			}
			else {
				// Unable to open image file
				// TODO: handle return value properly
			}
		}
	}

	void write_bmp_image(bmp_image& image, std::string filepath) {
		std::ofstream output_stream{ filepath.c_str(), std::ios_base::binary };

		if (output_stream) {
			if (image.info_header.bit_count == 32) {
				output_stream.write((const char*)&image.file_header, sizeof(bmp_file_header));
				output_stream.write((const char*)&image.info_header, sizeof(bmp_info_header));
				if (image.info_header.bit_count == 32) {
					output_stream.write((const char*)&image.color_header, sizeof(bmp_color_header));
				}
				output_stream.write((const char*)image.data.data(), image.data.size());
			}
			else if (image.info_header.bit_count == 24) {
				if (image.info_header.width % 4 == 0) {
					output_stream.write((const char*)&image.file_header, sizeof(bmp_file_header));
					output_stream.write((const char*)&image.info_header, sizeof(bmp_info_header));
					if (image.info_header.bit_count == 32) {
						output_stream.write((const char*)&image.color_header, sizeof(bmp_color_header));
					}
					output_stream.write((const char*)image.data.data(), image.data.size());
				}
				else {
					uint32_t temp_stride = image.row_stride;
					while (temp_stride % 4 != 0) {
						temp_stride++;
					}

					uint32_t new_stride = temp_stride;
					std::vector<uint8_t> padding_row(new_stride - image.row_stride);

					output_stream.write((const char*)&image.file_header, sizeof(bmp_file_header));
					output_stream.write((const char*)&image.info_header, sizeof(bmp_info_header));
					if (image.info_header.bit_count == 32) {
						output_stream.write((const char*)&image.color_header, sizeof(bmp_color_header));
					}

					for (int y = 0; y < image.info_header.height; ++y) {
						output_stream.write((const char*)(image.data.data() + image.row_stride * y), image.row_stride);
						output_stream.write((const char*)padding_row.data(), padding_row.size());
					}
				}
			}
			else {
				// The program can treat only 24 or 32 bits per pixel BMP files
				// TODO: handle return value properly
			}
		}
		else {
			// Unable to open the output image file.
			// TODO: handle return value properly
		}
	}
}