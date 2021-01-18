#include "element.hpp"

namespace hogl_ui {
	element::element(element* parent, const std::string& name)
		: parent(parent), name(name)
	{

	}

	element::~element() {

	}

	std::string element::get_name() const {
		return name;
	}

	void element::set_name(const std::string name){
		this->name = name;
	}
}
