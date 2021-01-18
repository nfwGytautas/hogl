#include "region.hpp"

namespace hogl_ui {

	region::region(element* parent, const std::string name)
		: element(parent, name), m_child(nullptr)
	{
		this->parent->add_child(this, "");
	}

	region::~region() {
		if (m_child) {
			delete m_child;
		}
	}

	void region::queue_vertices(const vertex_lists& vlist) {
		m_child->queue_vertices(vlist);
	}

	void region::render() {
		m_child->render();
	}

	void region::transform(const tinfo& t) {
		m_child->transform(t.combine(m_transform));
	}

	bool region::add_child(element* e, const std::string& region) {
		m_child = e;
		return true;
	}

	void region::set_transform(const tinfo& t) {
		m_transform = t;
	}

	tinfo& region::get_transform() {
		return m_transform;
	}

}
