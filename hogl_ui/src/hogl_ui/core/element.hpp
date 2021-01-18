#pragma once

#include <vector>
#include <string>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_ui/core/vertex_list.hpp"
#include "hogl_ui/core/transform.hpp"

namespace hogl_ui {

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
		 * @brief Sends a transform command which contains the ratios from 0.0f < val <= 1.0f that specify how to shift the position, scale, etc. of the element
		 * @param t Transform information
		*/
		virtual void transform(const tinfo& t) = 0;

		/**
		 * @brief Adds a child to the container, the container takes ownership of the element
		 * @param e Element to add
		 * @param region In region based layouts this is what lets the container know where to add the element
		 * @return True if element inserted, false otherwise
		*/
		virtual bool add_child(element* e, const std::string& region) { return false; }

		std::string get_name() const;
		void set_name(const std::string name);
	protected:
		element* parent = nullptr;
		std::string name = "";
	};

}
