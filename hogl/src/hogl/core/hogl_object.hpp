#pragma once

/**
 * @brief This file contains low level objects that can be used in hogl, all of these objects can be
 * made using hogl builders or by hand with ids that are created using OpenGL
*/

#include <vector>
#include <unordered_map>
#include <string>

#include "hogl/hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief Mesh object, that can be made using a builder, or filled by hand with user created OpenGL ids and passed into hogl systems.
 * If you are planning on creating these by hand than freeing of these resources are managed by you, otherwise use hogl_new_mesh and hogl_free_mesh functions.
*/
struct HOGL_API hogl_mesh
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
};

/**
 * @brief Shader object, that can be made using a builder, or filled by hand with user created OpenGL id and uniform mapping.
 * If you are planning on creating these by hand than freeing of these resources are managed by you, otherwise use hogl_new_shader and hogl_free_shader functions.
*/
struct HOGL_API hogl_shader
{
	/**
	 * @brief Shader used by this object
	*/
	unsigned int shader_id = 0;
};

/**
 * @brief This struct is used to specify ubo bind ranges
*/
struct HOGL_API hogl_ubo_bind_range
{
	/**
	 * @brief Bind block id
	*/
	unsigned int block_id = 0xFFFFFFFF;

	/**
	 * @brief UBO offset
	*/
	size_t offset = 0;

	/**
	 * @brief Bind range size
	*/
	size_t size = 0;
};

/**
 * @brief Uniform Buffer Object, that is used to pass data to a shader.
 * If you are planning on creating these by hand than freeing of these resources are managed by you, otherwise use hogl_new_ubo and hogl_free_ubo functions.
*/
struct HOGL_API hogl_ubo
{
	/**
	 * @brief OpenGL id of this ubo
	*/
	unsigned int ubo_id = 0;

	/**
	 * @brief UBO bind ranges
	*/
	std::vector<hogl_ubo_bind_range> bind_ranges;

	/**
	 * @brief Total allocated size
	*/
	size_t allocated = 0;
};

/**
 * @brief Struct used to specify shader layout, used when specifying ubo or sampler layout for the shader
*/
struct HOGL_API hogl_shader_layout
{
	/**
	 * @brief Map of entries blocks
	*/
	std::unordered_map<std::string, int> entries;
};

/**
 * @brief Texture, that is used to render images on meshes.
 * If you are planning on creating these by hand than freeing of these resources are managed by you, otherwise use hogl_new_texture and hogl_free_texture functions.
*/
struct HOGL_API hogl_texture
{
	/**
	 * @brief ID of this texture
	*/
	unsigned int texture_id = 0;

	/**
	 * @brief Texture slot
	*/
	unsigned int slot = 0;
};

/**
 * @brief Set ubo data, by default this function will set the entire ubo data
 * @param ubo hogl_ubo object
 * @param data Data to set
 * @param offset Where to set the data
 * @param size Size of the data
*/
HOGL_API void hogl_ubo_data(hogl_ubo* ubo, void* data, size_t offset = 0, size_t size = 0);

HOGL_NSPACE_END
