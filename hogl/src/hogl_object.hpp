#pragma once

#include <vector>

#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief Struct representing a single object in render space, this might be a cube, sphere or a complex model
*/
struct hogl_object
{
	/**
	 * @brief Vertex Buffer Objects of this object
	*/
	std::vector<unsigned int> vbo_ids;

	/**
	 * @brief Vertex Array Object of this object
	*/
	unsigned int vao_id = 0;

	/**
	 * @brief The count of vertices to render
	*/
	unsigned int vertice_count = 0;

	/**
	 * @brief Element Buffer Object id, if this is not 0 then the renderer will draw this as a indexed object
	*/
	unsigned int ebo_id = 0;

	/**
	 * @brief Shader used by this object
	*/
	unsigned int shader_id = 0;
};

HOGL_NSPACE_END
