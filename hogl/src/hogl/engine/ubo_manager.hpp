#pragma once

#include <unordered_map>
#include <vector>

#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/ubo.hpp"
#include "hogl/engine/standard_ubos.hpp"

namespace hogl {

	/**
	 * @brief Class used to manage uniform buffer objects
	*/
	class HOGL_CPP_API ubo_manager {
	public:
		/**
		 * @brief Creates the manager instance and creates the standard ubos that can then be reached through std_pkg function
		*/
		ubo_manager();
		~ubo_manager();

		/**
		 * @brief Returns the standard uniform buffer object package that hogl provides
		 * @return Reference to std_ubo_pkg structure
		*/
		relay_ptr<std_ubo_pkg> std_pkg();
	private:
		ref<std_ubo_pkg> m_stdPkg;
		std::vector<ref<ubo>> m_ubos;
	};

}
