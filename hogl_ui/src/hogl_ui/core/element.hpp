#pragma once

#include <vector>
#include <string>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/vertex_list.hpp"
#include "hogl_ui/core/transform.hpp"

namespace hogl_ui {

	/**
	 * @brief Simple union to parse hex color values into rgb
	*/
	union color {
		unsigned int hex;
		struct {
			unsigned char b, g, r;
		};
	};

	/**
	 * @brief Interface for all elements
	*/
	class HOGL_UI_API element {
	public:
		element(element* parent, const std::string& name);
		virtual ~element();

		/**
		 * @brief In this method the element needs to enqueue its vertices to the list
		 * @param vlist Vertex list to append to
		*/
		virtual void queue_vertices(const vertex_lists& vlist) = 0;

		/**
		 * @brief Override this method if you want custom OpenGL rendering
		 * in the element, otherwise prefer using the vertex list and update it,
		 * since that will usually result in better performance
		*/
		virtual void render() = 0;

		/**
		 * @brief Forces the element to recalculate its vertices using new transform information
		*/
		virtual void transform() = 0;

		/**
		 * @brief Adds a child to the container, the container takes ownership of the element
		 * @param e Element to add
		 * @param region In region based layouts this is what lets the container know where to add the element
		 * @return True if element inserted, false otherwise
		*/
		virtual bool add_child(element* e, const std::string& region) { return false; }

		std::string get_name() const;
		void set_name(const std::string name);

		/**
		 * @brief Sets the transform of the region
		 * @param t New transform
		*/
		void set_transform(const tinfo& t);

		/**
		 * @brief Returns a reference to the region transformation
		 * @return Reference to region transform
		*/
		tinfo& get_transform();

		/**
		 * @brief Sets the element background to the one specified
		 * @param c 
		*/
		virtual void set_background(color c) {}

		element* get_parent();
	protected:
		element* p_parent = nullptr;
		std::string p_name = "";
		tinfo p_transform;
	};

}
