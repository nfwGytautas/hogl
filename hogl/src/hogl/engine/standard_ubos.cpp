#include "standard_ubos.hpp"

namespace hogl {

	void std_ubo_pkg::attach_to(relay_ptr<shader> shader) {
		shader->ubo_attach(per_pass.as<ubo>());
		shader->ubo_attach(camera.as<ubo>());
		shader->ubo_attach(light.as<ubo>());
	}

}
