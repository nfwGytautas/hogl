#include "scene.hpp"

#include <entt/entt.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "hogl/entity/components.hpp"
#include "hogl/engine/rendering/draw_call.hpp"

namespace hogl {
    scene::scene(relay_ptr<std_ubo_pkg> ubos, relay_ptr<instance_data> idata)
        : m_ubos(ubos), m_idata(idata)
    {}

    void scene::set_camera(const camera& cam) {
        m_camera = cam;
    }

    void hogl::scene::render() {
        std::vector<draw_call> calls;

        m_ubos->per_pass->data.view = this->m_camera.compute_view();
        m_ubos->per_pass->data.projection = this->m_camera.compute_projection();
        m_ubos->per_pass->update();

        m_ubos->camera->data.position = m_camera.get_position();
        m_ubos->camera->update();

        // Lights
        auto light_group = m_sreg.group<light_component>(entt::get<transform_component>);
        size_t active = 0;
        for (auto entity : light_group) {
            auto [light, transform] = light_group.get(entity);

            m_ubos->light->data.position[active] = glm::vec4(transform.position, 0.0f);
            m_ubos->light->data.colors[active] = glm::vec4(light.color, 0.0f);

            // TODO: Check if light active
            active++;
        }
        m_ubos->light->data.count = active;
        m_ubos->light->update();

        auto group = m_sreg.group<mesh_component, renderer_component>(entt::get<transform_component>);
        for (auto entity : group) {
            auto [mesh, renderer, transform] = group.get(entity);

            glm::mat4 transformation = 
                glm::translate(glm::mat4(1.0f), transform.position) * 
                glm::toMat4(glm::quat(transform.rotation)) * 
                glm::scale(glm::mat4(1.0f), transform.scale);

            // Create a new call
            if (calls.size() == 0) {
                draw_call dc = draw_call(mesh.mesh, renderer.material);
                dc.transformations.push_back(transformation);
                calls.push_back(dc);
            }
            else {
                // TODO: Optimize?
                for (int i = 0; i < calls.size(); i++) {
                    if (calls[i].mesh == mesh.mesh && calls[i].material == renderer.material) {
                        calls[i].transformations.push_back(transformation);
                    }
                }
            }
        }

        for (draw_call& dc : calls) {
            m_idata->push_transformations(dc.transformations);

            dc.material->bind();
            dc.mesh->bind();
            m_idata->bind();
            dc.mesh->render_instanced(dc.transformations.size());
        }
    }

    entity hogl::scene::spawn_entity() {
        return entity(this, m_sreg.create());
    }

}
