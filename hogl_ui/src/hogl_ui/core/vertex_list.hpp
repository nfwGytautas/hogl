#pragma once

#include <vector>
#include <string>
#include "hogl_ui/shared/hogl_ui_def.hpp"
#include "hogl_core/graphics/hogl_gl_primitive.h"

namespace hogl_ui {

	/**
	 * @brief Class used to store each ui element vertices so it could be
	 * batched in a single draw call, for complex object overriding the render
	 * might be mandatory
	*/
	class HOGL_UI_API vertex_list {
	public:
		vertex_list();
		~vertex_list();

		/**
		 * @brief Loads a shader for the vertex list
		 * @param vertex Vertex shader source
		 * @param fragment Fragment shader source
		*/
		void load_shader(const std::string& vertex, const std::string& fragment);

		/**
		 * @brief Adds the specified vector to the vertices list
		 * @param data Vertices to add
		 * @param indices Indices to add
		*/
		void add_range(const std::vector<float>& data, const std::vector<unsigned int>& indices);

		/**
		 * @brief Renders the vertex list to the active context/window
		*/
		void render();
	private:
		hogl_vao* m_vao = nullptr;
		hogl_vbo* m_vbo = nullptr;
		hogl_vbo* m_ebo = nullptr;
		hogl_shader* m_shader = nullptr;

		std::vector<float> m_vertices = {};
		std::vector<unsigned int> m_indices = {};

		// Last frame vertices size, used to check if the vbo needs to be expanded
		size_t m_lfsize = 0;
	};

	/**
	 * @brief Vertex lists that are provided by hogl, main differences being their
	 * shaders
	*/
	struct vertex_lists {
		/**
		 * @brief Simple quad render
		*/
		vertex_list* quad;
	};
}
