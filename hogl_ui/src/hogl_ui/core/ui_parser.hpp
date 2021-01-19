#pragma once

#include <string>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/element.hpp"

namespace hogl_ui {

	/**
	 * @brief Class used to parse entire xml document and construct a ui
	*/
	class HOGL_UI_API ui_parser {
	public:
		ui_parser(element* root);

		/**
		 * @brief Parses a file and adds the elements to root
		 * @param file File to parse
		 * @return True if layout parsed successfully, false otherwise 
		*/
		bool parse(const std::string& file);
	private:
		element* m_root;
	};

}
