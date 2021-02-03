#pragma once

#include <string>
#include "hogl/shared/hogl_def.hpp"

namespace hogl {

	/**
	 * @brief Asset manager provided by the hogl engine
	*/
	class HOGL_CPP_API asset_manager {
	public:
		
		/**
		 * @brief Reads the specified text file and returns it's contents
		 * @param path Path to the file
		 * @return Contents of the file
		*/
		std::string load_txt(const std::string& path);
	private:

	};

}
