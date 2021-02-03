#include "ubo_manager.hpp"

namespace hogl {
	ubo_manager::ubo_manager() 
		: m_stdPkg(new std_ubo_pkg())
	{
		ref<per_pass_ubo> ppubo = new per_pass_ubo();

		m_stdPkg->per_pass = ppubo.relay();

		m_ubos.push_back(ppubo.as<ubo>());
	}

	ubo_manager::~ubo_manager() {}

	relay_ptr<std_ubo_pkg> ubo_manager::std_pkg() {
		return m_stdPkg.relay();
	}
}
