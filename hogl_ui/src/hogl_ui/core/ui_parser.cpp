#include "ui_parser.hpp"

#include <unordered_map>
#include <functional>

#include <pugixml.hpp>

#include "hogl_core/shared/hogl_log.h"
#include "hogl_ui/elements/free_layout.hpp"
#include "hogl_ui/elements/region.hpp"
#include "hogl_ui/elements/button.hpp"

namespace hogl_ui {

	bool parse_object_attributes(element* e, pugi::xml_node& object);
	bool parse_button(element* parent, pugi::xml_node& node);
	bool parse_region(element* parent, pugi::xml_node& node);
	bool parse_layout(element* parent, pugi::xml_node& node);

	/**
	 * @brief Maps a node name to creation function
	 * @param parent Parent element
	 * @param node XML element
	 * @return True if no errors occurred, false otherwise
	*/
	bool child_map(element* parent, pugi::xml_node& node) {
		const pugi::char_t* name = node.name();

		if (strcmp(name, "button") == 0) {
			return parse_button(parent, node);
		} 
		else if (strcmp(name, "region") == 0) {
			return parse_region(parent, node);
		}
		else if (strcmp(name, "layout") == 0) {
			return parse_region(parent, node);
		}
		else {
			hogl_log_warn("Unknown element %s", name);
			return false;
		}
	}

	/**
	 * @brief Parses attributes of an object
	 * @param e Element to parse into
	 * @param object Object where to read from
	 * @return True if no errors occurred, false otherwise
	*/
	bool parse_object_attributes(element* e, pugi::xml_node& object) {
		tinfo& t = e->get_transform();

		for (pugi::xml_attribute attr : object.attributes()) {
			const pugi::char_t* name = attr.name();			

			if (strcmp(name, "pos:x") == 0) {
				t.rel_x = attr.as_float();
			}
			else if (strcmp(name, "pos:y") == 0) {
				t.rel_y = attr.as_float();
			}
			else if (strcmp(name, "width") == 0) {
				t.rel_width = attr.as_float();
			}
			else if (strcmp(name, "height") == 0) {
				t.rel_height = attr.as_float();
			}
			else if (strcmp(name, "background") == 0) {
				color c;
				std::string hex = attr.as_string();
				unsigned int hexuint = std::stoul(hex.substr(1, 6), nullptr, 16);
				c.hex = hexuint;
				e->set_background(c);
			}
		}

		return true;
	}

	/**
	 * @brief Parse a button
	 * @param parent Parent element
	 * @param node XML representation
	 * @return True if no errors occurred, false otherwise
	*/
	bool parse_button(element* parent, pugi::xml_node& node) {
		const pugi::char_t* name = node.attribute("name").value();
		button* b1 = new button(parent, name);
		parse_object_attributes(b1, node);
		return true;
	}

	/**
	 * @brief Parse a region
	 * @param parent Parent element
	 * @param node XML representation
	 * @return True if no errors occurred, false otherwise
	*/
	bool parse_region(element* parent, pugi::xml_node& node) {
		const pugi::char_t* name = node.attribute("name").value();
		hogl_ui::region* r = new hogl_ui::region(parent, name);

		// Transform
		if (!parse_object_attributes(r, node)) {
			return false;
		}

		for (pugi::xml_node node : node.children()) {
			if (!child_map(r, node)) {
				return false;
			}
		}
		return true;
	}

	/**
	 * @brief Parse a layout
	 * @param parent Parent element
	 * @param node XML representation
	 * @return True if no errors occurred, false otherwise
	*/
	bool parse_layout(element* parent, pugi::xml_node& node) {
		element* el = nullptr;

		// Get layout type
		const pugi::char_t* type = node.attribute("type").value();

		if (strcmp(type, "free") == 0) {
			const pugi::char_t* name = node.attribute("name").value();
			el = new free_layout(parent, name);
		}
		else {
			hogl_log_error("Unknown layout type '%s'", type);
			return false;
		}

		parse_object_attributes(el, node);

		// Next should be regions
		for (pugi::xml_node region : node.children("region")) {
			// Elements
			if (!parse_region(el, region)) {
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
