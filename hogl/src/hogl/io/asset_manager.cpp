#include "asset_manager.hpp"

#include <fstream>

namespace hogl {
	std::string asset_manager::load_txt(const std::string& path) {
		std::ifstream t(path);
		t.seekg(0, std::ios::end);
		size_t size = t.tellg();
		std::string buffer(size, ' ');
		t.seekg(0);
		t.read(&buffer[0], size);
		return buffer;
	}
}
