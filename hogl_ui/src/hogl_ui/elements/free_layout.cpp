#include "free_layout.hpp"

#include "hogl_core/shared/hogl_log.h"

namespace hogl_ui {

    free_layout::free_layout(element* parent, const std::string& name)
        : element(parent, name)
    {
        parent->add_child(this, "root.layout");
    }

    free_layout::~free_layout() {
        for (element* c : m_regions) {
            delete c;
        }
    }

    void free_layout::queue_vertices(const vertex_lists& vlist) {
        for (element* c : m_regions) {
            c->queue_vertices(vlist);
        }
    }

    void free_layout::render() {
        for (element* c : m_regions) {
            c->render();
        }
    }

    void free_layout::transform(const tinfo& t) {
        for (element* c : m_regions) {
            c->transform(t);
        }
    }

    bool free_layout::add_child(element* e, const std::string& region) {
        m_regions.push_back(static_cast<hogl_ui::region*>(e));
        return true;
    }
}
