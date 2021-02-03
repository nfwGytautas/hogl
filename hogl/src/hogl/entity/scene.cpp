#include "scene.hpp"

#include <entt/entt.hpp>
#include "hogl/entity/components.hpp"

namespace hogl {
    scene::scene(relay_ptr<std_ubo_pkg> ubos)
        : m_ubos(ubos)
    {}

    void scene::set_camera(const camera& cam) {
        m_camera = cam;
    }

    void hogl::scene::render() {
        m_ubos->per_pass->data.view = this->m_camera.compute_view();
        m_ubos->per_pass->data.projection = this->m_camera.compute_projection();
        m_ubos->per_pass->update();

        auto group = m_sreg.group<mesh_component, renderer_component>(entt::get<transform_component>);
        for (auto entity : group) {
            auto [mesh, renderer, transform] = group.get(entity);

            // TODO: Transform
            renderer.material->bind();
            mesh.mesh->bind();
            mesh.mesh->render();
        }
    }

    entity hogl::scene::spawn_entity() {
        return entity(this, m_sreg.create());
    }

}
