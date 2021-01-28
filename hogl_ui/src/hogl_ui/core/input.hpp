#pragma once

#include <vector>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_core/graphics/hogl_wnd.h"

namespace hogl_ui {

	/**
	 * @brief Handles input for the framework
	*/
	class HOGL_UI_API input_manager {
	public:
		input_manager();
		~input_manager();

		/**
		 * @brief Adds a window pointer to the manager
		 * @param pwi Window manager pointer
		*/
		void add_wi(hogl_wi* pwi);

		/**
		 * @brief Sends close callback
		*/
		bool on_close();
	private:
		std::vector<hogl_wi*> m_wis;
	};
}
