#include "ui_parser.hpp"

#include <pugixml.hpp>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_ui/elements/free_layout.hpp"
#include "hogl_ui/elements/region.hpp"
#include "hogl_ui/elements/button.hpp"

namespace hogl_ui {

	bool parse_region_transform(tinfo& t, pugi::xml_node& region) {
		for (pugi::xml_attribute attr : region.attributes()) {
			const pugi::char_t* name = attr.name();

			if (strcmp(name, "pos:x") == 0) {
				t.x_offset = attr.as_float();
			}
			else if (strcmp(name, "pos:y") == 0) {
				t.y_offset = attr.as_float();
			}
			else if (strcmp(name, "pos:z") == 0) {
				t.z_offset = attr.as_float();
			}
			else if (strcmp(name, "scale:x") == 0) {
				t.x_scale = attr.as_float();
			}
			else if (strcmp(name, "scale:y") == 0) {
				t.y_scale = attr.as_float();
			}
			else if (strcmp(name, "scale:z") == 0) {
				t.z_scale = attr.as_float();
			}
		}

		return true;
	}

	bool parse_button(element* parent, pugi::xml_node& node) {
		const pugi::char_t* name = node.attribute("name").value();
		button* b1 = new button(parent, name);
		return true;
	}

	bool parse_region_elements(region* r, pugi::xml_node& region) {
		for (pugi::xml_node node : region.children()) {
			const pugi::char_t* name = node.name();

			if (strcmp(name, "button") == 0) {
				parse_button(r, node);
			}
			else {
				hogl_log_warn("Unknown element %s", name);
			}
		}
		return true;
	}

	bool parse_layout(element* parent, pugi::xml_node& layout) {
		element* el = nullptr;

		// Get layout type
		const pugi::char_t* type = layout.attribute("type").value();

		if (strcmp(type, "free") == 0) {
			const pugi::char_t* name = layout.attribute("name").value();
			el = new free_layout(parent, name);
		}
		else {
			hogl_log_error("Unknown layout type '%s'", type);
			return false;
		}

		// Next should be regions
		for (pugi::xml_node region : layout.children("region")) {
			const pugi::char_t* name = region.attribute("name").value();
			hogl_ui::region* r = new hogl_ui::region(el, name);

			// Transform
			tinfo& t = r->get_transform();
			if (!parse_region_transform(t, region)) {
				return false;
			}

			// Elements
			if (!parse_region_elements(r, region)) {
				return false;
			}
		}

		return true;
	}

	bool parse_ui(element* root, pugi::xml_document& doc) {
		// First element is always layout
		pugi::xml_node layout = doc.child("layout");

		if (layout == nullptr) {
			hogl_log_error("First element must always be a layout object");
			return false;
		}

		return parse_layout(root, layout);
	}

    ui_parser::ui_parser(element* root) 
        : m_root(root)
    {
    }

    bool ui_parser::parse(const std::string& file) {
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(file.c_str());

		if (!result) {
			hogl_log_error("%s", result.description());
			return false;
		}

		return parse_ui(m_root, doc);
    }

}
