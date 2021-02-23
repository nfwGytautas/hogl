#pragma once

#include "hogl/shared/hogl_def.hpp"
#include "hogl_core/hogl_core.h"

namespace hogl {
	/**
	 * @brief Class used to wrap hogl C window
	*/
	class HOGL_CPP_API window {
	public:
		window();
		virtual ~window();

		void swap_buffer();
		hogl_wi* get_wi();
	private:
		hogl_wnd* m_hwindow = nullptr;
	};
}
