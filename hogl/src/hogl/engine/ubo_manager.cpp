#include "ubo_manager.hpp"

namespace hogl {
	ubo_manager::ubo_manager() 
		: m_stdPkg(new std_ubo_pkg())
	{
		ref<per_pass_ubo> ppubo = new per_pass_ubo();
		ref<camera_ubo> cubo = new camera_ubo();
		ref<light_ubo> lubo = new light_ubo();

		m_stdPkg->per_pass = ppubo.relay();
		m_stdPkg->camera = cubo.relay();
		m_stdPkg->light = lubo.relay();

		m_ubos.push_back(ppubo.as<ubo>());
		m_ubos.push_back(cubo.as<ubo>());
		m_ubos.push_back(lubo.as<ubo>());
	}

	ubo_manager::~ubo_manager() {}

	relay_ptr<std_ubo_pkg> ubo_manager::std_pkg() {
		return m_stdPkg.relay();
	}
}
