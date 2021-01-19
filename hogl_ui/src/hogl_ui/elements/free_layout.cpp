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

    void free_layout::transform() {
        // Update object transform
        tinfo& et = this->get_transform();
        tinfo& pt = this->get_parent()->get_transform();

        et.actual_width = pt.actual_width * et.rel_width;
        et.actual_height = pt.actual_height * et.rel_height;

        et.actual_x = pt.actual_x + pt.actual_width * (et.rel_x);
        et.actual_y = pt.actual_y + pt.actual_height * (et.rel_y);

        // The layout does not render anything just passes the command further
        for (element* c : m_regions) {
            c->transform();
        }
    }

    bool free_layout::add_child(element* e, const std::string& region) {
        m_regions.push_back(static_cast<hogl_ui::region*>(e));
        return true;
    }
}
