#include "root.hpp"

namespace hogl_ui {
	root::root() 
		: element(nullptr, "root")
	{
	}

	root::~root() {
		if (m_layout) {
			delete m_layout;
		}
	}

	void root::queue_vertices(const vertex_lists& vlist) {
		m_layout->queue_vertices(vlist);
	}

	void root::render() {
		m_layout->render();
	}

	void root::transform() {
		// Root will always have a screen width and height of the window and cannot calculate it otherwise
		m_layout->transform();
	}

	bool root::add_child(element* e, const std::string& region) {
		if (region == "root.layout") {
			m_layout = e;
			return true;
		}

		return false;
	}

}