#include "entity.hpp"

#include "hogl/entity/components.hpp"

namespace hogl {

	entity::entity(relay_ptr<scene> scene, entt::entity id)
		: m_scene(scene), m_id(id)
	{ 
		// By default all entities have a transform component
		this->add_component<transform_component>();
	}

	relay_ptr<scene> entity::get_scene() {
		return m_scene;
	}

}
