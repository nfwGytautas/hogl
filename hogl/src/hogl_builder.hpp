#pragma once

#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl object builder used to create or alter hogl objects, 
 * make sure that a OpenGL context is active on the current thread
*/
class hogl_obj_builder
{
public:
	/**
	 * @brief Create a builder for the specified object
	 * @param object Instance to a hogl_object to build
	*/
	hogl_obj_builder(hogl_object* object);

	/**
	 * @brief Initialize the hogl object
	*/
	hogl_obj_builder& init();

	/**
	 * @brief Sets the current working vbo to the specified one with the id, make sure to call set_vbo_type before hand
	 * @param type VBO type
	 * @param id ID of the vbo
	*/
	hogl_obj_builder& set_vbo(hogl_vbo_type type, unsigned int id);

	/**
	 * @brief Adds a vertex buffer object to the hogl_object, succeeding calls will alter this vbo, make sure to call set_vbo_type before hand
	 * @param type VBO type
	*/
	hogl_obj_builder& add_vbo(hogl_vbo_type type);

	/**
	 * @brief Add data to the current vbo, make sure to have a vbo set and its usage and type also specified
	 * @param usage VBO data usage
	 * @param size Size of the data
	 * @param data Pointer to the new buffer data
	*/
	hogl_obj_builder& add_vbo_data(hogl_vbo_usage usage, size_t size, void* data);

	/**
	 * @brief Set the active vbo attribute pointer, make sure that either a vbo was set or added before this call
	 * @param slot Attribute pointer slot
	 * @param etype Element type, check hogl_ap_type for more information
	 * @param count Number of elements for this slot, check hogl_ap_count for more information
	 * @param stride Spacing between element entries
	 * @param offset Position where the element starts in the vbo
	*/
	hogl_obj_builder& set_vbo_ap(unsigned int slot, hogl_ap_type etype, hogl_ap_count count, unsigned int stride, unsigned int offset);

	/**
	 * @brief Sets object vertices count to the one specified, not needed if a element buffer is provided
	 * @param count Amount of vertices to render
	*/
	hogl_obj_builder& set_vertices_count(unsigned int count);

	/**
	 * @brief Set shader for this object to the one specified
	 * @param shader ID of the shader
	*/
	hogl_obj_builder& set_shader(unsigned int shader);

	/**
	 * @brief Create a new shader for this object
	*/
	hogl_obj_builder& add_shader();

	/**
	 * @brief Add and compile vertex shader source
	 * @param source Vertex shader source
	*/
	hogl_obj_builder& add_vertex_source(const char* source);

	/**
	 * @brief Add and compile fragment shader source
	 * @param source Fragment shader source
	*/
	hogl_obj_builder& add_fragment_source(const char* source);

	/**
	 * @brief Link the object shader
	*/
	hogl_obj_builder& link_shader();


private:
	hogl_object* m_object = nullptr;
	bool m_hasVBO = false;
	bool m_hasShader = false;
	hogl_vbo_type m_type = hogl_vbo_type::UNSET;
	unsigned int m_tempVert = 0;
	unsigned int m_tempFrag = 0;
};

HOGL_NSPACE_END
