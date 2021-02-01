#include "scene.hpp"

namespace hogl {

    void hogl::scene::render() {

    }

    entity hogl::scene::spawn_entity() {
        return entity(this, m_sreg.create());
    }

}
