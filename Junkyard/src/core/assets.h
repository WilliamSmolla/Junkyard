#pragma once
#include <string>

struct asset {
	asset(std::string path)
		: filepath{ path } {}

	std::string filepath;
};
