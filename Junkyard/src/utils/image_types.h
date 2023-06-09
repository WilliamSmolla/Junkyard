#pragma once
#include <inttypes.h>
#include <string>
#include <vector>

// ----- BMP -----
#pragma pack(push, 1)
struct bmp_file_header {
	uint16_t file_type = 0x4D42;
	uint32_t file_size = 0;
	uint16_t reserved_one = 0;
	uint16_t reserved_two = 0;
	uint32_t offset_data = 0;
};
#pragma pack(pop)

struct bmp_info_header {
	uint32_t size = 0;
	int32_t width = 0;                   
	int32_t height = 0;                  
	uint16_t planes = 1;                 
	uint16_t bit_count = 0;              
	uint32_t compression = 0;            
	uint32_t size_image = 0;             
	int32_t x_pixels_per_meter = 0;
	int32_t y_pixels_per_meter = 0;
	uint32_t colors_used = 0;            
	uint32_t colors_important = 0;
};

struct bmp_color_header {
	uint32_t red_mask = 0x00ff0000;        
	uint32_t green_mask = 0x0000ff00;      
	uint32_t blue_mask = 0x000000ff;       
	uint32_t alpha_mask = 0xff000000;      
	uint32_t color_space_type = 0x73524742;
	uint32_t unused[16]{ 0 };
};

struct bmp_image {
	bmp_file_header file_header;
	bmp_info_header info_header;
	bmp_color_header color_header;
	std::vector<uint8_t> data;
	uint32_t row_stride = 0;
};
