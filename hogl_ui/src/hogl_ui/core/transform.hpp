#pragma once

#include "hogl_ui/shared/hogl_ui_def.hpp"

namespace hogl_ui {

	/**
	 * @brief Transformation information class
	*/
	class HOGL_UI_API tinfo {
	public:
		/**
		 * @brief Actual width
		*/
		float actual_width = 0.0f;

		/**
		 * @brief Actual height
		*/
		float actual_height = 0.0f;

		/**
		* @brief Actual x position value, from -1 to 1, calculated using actual_width
		*/
		float actual_x = 0.0f;

		/**
		* @brief Actual y position value, from -1 to 1, calculated using actual_height
		*/
		float actual_y = 0.0f;

		/**
		 * @brief Percent of parent width
		*/
		float rel_width = 1.0f;

		/**
		 * @brief Percent of parent height
		*/
		float rel_height = 1.0f;

		/**
		* @brief Percent of x position
		*/
		float rel_x = 0.0f;

		/**
		* @brief Percent of y position
		*/
		float rel_y = 0.0f;

		tinfo();
	};

}
