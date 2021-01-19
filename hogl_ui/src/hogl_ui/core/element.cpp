#include "element.hpp"

namespace hogl_ui {
	element::element(element* parent, const std::string& name)
		: p_parent(parent), p_name(name)
	{

	}

	element::~element() {

	}

	std::string element::get_name() const {
		return p_name;
	}

	void element::set_name(const std::string name){
		this->p_name = name;
	}

	void element::set_transform(const tinfo& t) {
		p_transform = t;
	}

	tinfo& element::get_transform() {
		return p_transform;
	}

	element* element::get_parent() {
		return p_parent;
	}
}
