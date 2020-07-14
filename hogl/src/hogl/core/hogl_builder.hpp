#pragma once

#include "hogl/hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl mesh builder used to create or alter hogl objects,
 * make sure that a OpenGL context is active on the current thread
*/
class HOGL_API hogl_bldr_mesh
{
public:
	/**
	 * @brief Create a builder for the specified object
	 * @param mesh Instance to a hogl_mesh to build
	*/
	hogl_bldr_mesh(hogl_mesh* mesh);

	/**
	 * @brief Sets the current working vbo to the specified one with the id, make sure to call set_vbo_type before hand
	 * @param type VBO type
	 * @param id ID of the vbo
	*/
	hogl_bldr_mesh& set_vbo(hogl_vbo_type type, unsigned int id);

	/**
	 * @brief Adds a vertex buffer object to the hogl_object, succeeding calls will alter this vbo, make sure to call set_vbo_type before hand
	 * @param type VBO type
	*/
	hogl_bldr_mesh& add_vbo(hogl_vbo_type type);

	/**
	 * @brief Add data to the current vbo, make sure to have a vbo set and its usage and type also specified
	 * @param usage VBO data usage
	 * @param size Size of the data
	 * @param data Pointer to the new buffer data
	*/
	hogl_bldr_mesh& add_vbo_data(hogl_vbo_usage usage, size_t size, void* data);

	/**
	 * @brief Set the active vbo attribute pointer, make sure that either a vbo was set or added before this call
	 * @param slot Attribute pointer slot
	 * @param etype Element type, check hogl_ap_type for more information
	 * @param count Number of elements for this slot, check hogl_ap_count for more information
	 * @param stride Spacing between element entries
	 * @param offset Position where the element starts in the vbo
	*/
	hogl_bldr_mesh& set_vbo_ap(unsigned int slot, hogl_ap_type etype, hogl_ap_count count, unsigned int stride, unsigned int offset);

	/**
	 * @brief Sets object vertices count to the one specified, not needed if a element buffer is provided
	 * @param count Amount of vertices to render
	*/
	hogl_bldr_mesh& set_vertices_count(unsigned int count);

	/**
	 * @brief Returns the instance that this builder has built
	 * @return Pointer to a hogl_mesh instance
	*/
	hogl_mesh* ptr();
private:
	hogl_mesh* m_mesh = nullptr;
	bool m_hasVBO = false;
	hogl_vbo_type m_type = hogl_vbo_type::UNSET;
};

/**
 * @brief hogl shader builder used to create or alter hogl objects,
 * make sure that a OpenGL context is active on the current thread
*/
class HOGL_API hogl_bldr_shader
{
public:
	/**
	 * @brief Create a builder for the specified object
	 * @param shader Instance to a hogl_shader to build
	*/
	hogl_bldr_shader(hogl_shader* shader);

	/**
	 * @brief Set shader for this object to the one specified
	 * @param shader ID of the shader
	*/
	hogl_bldr_shader& set_shader(unsigned int shader);

	/**
	 * @brief Create a new shader for this object
	*/
	hogl_bldr_shader& add_shader();

	/**
	 * @brief Set vertex shader part of the shader program
	 * @param shader OpenGL id of the shader
	*/
	hogl_bldr_shader& set_vertex_shader(unsigned int shader);

	/**
	 * @brief Add and compile vertex shader source
	 * @param source Vertex shader source
	*/
	hogl_bldr_shader& add_vertex_source(const char* source);

	/**
	 * @brief Set fragment shader part of the shader program
	 * @param shader OpenGL id of the shader
	*/
	hogl_bldr_shader& set_fragment_shader(unsigned int shader);

	/**
	 * @brief Add and compile fragment shader source
	 * @param source Fragment shader source
	*/
	hogl_bldr_shader& add_fragment_source(const char* source);

	/**
	 * @brief Link the object shader
	*/
	hogl_bldr_shader& link_shader();

	/**
	 * @brief Applies a UBO layout to this shader, only one layout can be applied at a given time
	 * @param layout hogl_shader_layout instance of the layout
	*/
	hogl_bldr_shader& ubo_layout(const hogl_shader_layout& layout);

	/**
	 * @brief Applies a sampler layout to this shader, only one layout can be applied at a given time
	 * @param layout hogl_shader_layout instance of the layout
	*/
	hogl_bldr_shader& sampler_layout(const hogl_shader_layout& layout);

	/**
	 * @brief Returns the instance that this builder has built
	 * @return Pointer to a hogl_shader instance
	*/
	hogl_shader* ptr();
private:
	hogl_shader* m_shader = nullptr;
	bool m_hasShader = false;
	unsigned int m_tempVert = 0;
	unsigned int m_tempFrag = 0;
};

/**
 * @brief hogl ubo builder used to create or alter hogl objects,
 * make sure that a OpenGL context is active on the current thread
*/
class HOGL_API hogl_bldr_ubo
{
public:
	/**
	 * @brief Create a builder for the specified object
	 * @param ubo Instance to a hogl_ubo to build
	*/
	hogl_bldr_ubo(hogl_ubo* ubo);

	/**
	 * @brief Set ubo for this object to the one specified
	 * @param ubo ID of the ubo
	*/
	hogl_bldr_ubo& set_ubo(unsigned int ubo);

	/**
	 * @brief Create a new ubo for this object
	*/
	hogl_bldr_ubo& add_ubo();

	/**
	 * @brief Allocates the specified size to the UBO
	 * @param size Amount of bytes to allocate
	*/
	hogl_bldr_ubo& allocate(size_t size);

	/**
	 * @brief Adds a bind range to this ubo, specify offset 0 and size to allocated size, to have the buffer bind entirely, make sure to call allocate before
	 * @param block Bind range block id
	 * @param offset Number of bytes where to start the range
	 * @param size Number of bytes to bind from offset
	*/
	hogl_bldr_ubo& add_range(unsigned int block, size_t offset, size_t size);

	/**
	 * @brief Returns the instance that this builder has built
	 * @return Pointer to a hogl_shader instance
	*/
	hogl_ubo* ptr();
private:
	hogl_ubo* m_ubo = nullptr;
};

/**
 * @brief hogl texture builder used to create or alter hogl objects,
 * make sure that a OpenGL context is active on the current thread
*/
class HOGL_API hogl_bldr_texture
{
public:
	/**
	 * @brief Create a builder for the specified object
	 * @param texture Instance to a hogl_texture to build
	*/
	hogl_bldr_texture(hogl_texture* texture);

	/**
	 * @brief Set texture for this object to the one specified
	 * @param texture ID of the texture
	*/
	hogl_bldr_texture& set_texture(unsigned int texture);

	/**
	 * @brief Create a new ubo for this object
	*/
	hogl_bldr_texture& add_texture();

	/**
	 * @brief Adds image data to the texture
	 * @param image_data Data to add to the texture
	*/
	hogl_bldr_texture& add_data(hogl_loader_image* image_data);

	/**
	 * @brief Generate mipmaps for this texture
	*/
	hogl_bldr_texture& generate_mipmap();

	/**
	 * @brief Set wrapping type for the axis
	 * @param axis Axis to set for
	 * @param type Type to set to
	*/
	hogl_bldr_texture& set_wrap(hogl_wrap_axis axis, hogl_wrap_type type);

	/**
	 * @brief Set mag filtering type
	 * @param type Filtering type
	*/
	hogl_bldr_texture& set_mag_filter(hogl_filter_type type);

	/**
	 * @brief Set min filtering type
	 * @param type Filtering type
	*/
	hogl_bldr_texture& set_min_filter(hogl_filter_type type);

	/**
	 * @brief Set texture slot
	 * @param slot Number specifying the texture slot that is in between 0 and HOGL_MAX_TEXTURE_SLOTS
	*/
	hogl_bldr_texture& slot(unsigned int slot);

	/**
	 * @brief Returns the instance that this builder has built
	 * @return Pointer to a hogl_shader instance
	*/
	hogl_texture* ptr();
private:
	hogl_texture* m_texture = nullptr;
	bool m_hasTexture = false;
};

/**
 * @brief Create a new mesh, the instance is inside the builder, make sure to call hogl_free after using the mesh
 * @return hogl_bldr_mesh instance with the new mesh object instance
*/
HOGL_API hogl_bldr_mesh hogl_new_mesh();

/**
 * @brief Create a new shader, the instance is inside the builder, make sure to call hogl_free after using the shader
 * @return hogl_bldr_shader instance with the new shader object instance
*/
HOGL_API hogl_bldr_shader hogl_new_shader();

/**
 * @brief Create a new ubo, the instance is inside the builder, make sure to call hogl_free after using the ubo
 * @return hogl_bldr_ubo instance with the new ubo instance
*/
HOGL_API hogl_bldr_ubo hogl_new_ubo();

/**
 * @brief Create a new texture, the instance is inside the builder, make sure to call hogl_free after using the texture
 * @return hogl_bldr_texture instance with the new texture instance
*/
HOGL_API hogl_bldr_texture hogl_new_texture();

/**
 * @brief Free a hogl_mesh object
 * @param mesh Pointer to a hogl_mesh object instance
*/
HOGL_API void hogl_free(hogl_mesh*& mesh);

/**
 * @brief Free a hogl_shader object
 * @param shader Pointer to a hogl_shader object instance
*/
HOGL_API void hogl_free(hogl_shader*& shader);

/**
 * @brief Free a hogl_ubo object
 * @param ubo Pointer to a hogl_ubo object instance
*/
HOGL_API void hogl_free(hogl_ubo*& ubo);

/**
 * @brief Free a hogl_texture object
 * @param texture Pointer to a hogl_texture object instance
*/
HOGL_API void hogl_free(hogl_texture*& texture);

HOGL_NSPACE_END
