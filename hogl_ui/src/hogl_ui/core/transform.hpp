#pragma once

#include "hogl_ui/shared/hogl_ui_def.hpp"

namespace hogl_ui {

	/**
	 * @brief Transformation information class
	*/
	class HOGL_UI_API tinfo {
	public:
		float x_offset = 0.0f;
		float y_offset = 0.0f;
		float z_offset = 0.0f;

		float x_scale = 1.0f;
		float y_scale = 1.0f;
		float z_scale = 1.0f;

		tinfo();
		tinfo(float xoff, float yoff, float zoff, float xscale, float yscale, float zscale);

		/**
		 * @brief Combines this transformation with the specified and returns a new one
		 * @param t Transformation to combine with
		 * @return Combined transformation
		*/
		tinfo combine(const tinfo& t) const;
	};

}
