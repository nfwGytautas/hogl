/**
* @brief hogl gl primitive file contains OpenGL primitives e.g. VAO, shader, texture, framebuffer, etc.
*/

#ifndef _HOGL_GL_PRIMITIVE_
#define _HOGL_GL_PRIMITIVE_

#include "hogl_core/shared/hogl_def.h"
#include <stdbool.h>

/**
 * @brief Vertex array object contains data about the object that is being rendered, it is made of
 * vertex buffers, that contains the actual data
*/
typedef struct _hogl_vao hogl_vao;

/**
 * @brief Texture is used to provide any type of image to GPU or query data from OpenGL and store into
 * a image
*/
typedef struct _hogl_texture hogl_texture;

/**
 * @brief Shader is used to control programs that are responsible for rendering vertex objects using
 * the specified textures
*/
typedef struct _hogl_shader hogl_shader;

/**
 * @brief Uniform buffer object is used to contain data that is passed to a GPU, by creating a ubo and then
 * binding it to shader block, this can be done inside a shader if OpenGL version is >=4.2 otherwise manual
 * block binding is necessary.
*/
typedef struct _hogl_ubo hogl_ubo;

/**
 * @brief Framebuffer can be used as a target where objects will be rendered into
*/
typedef struct _hogl_framebuffer hogl_framebuffer;

/**
 * @brief Renderbuffer that can be used as a constant framebuffer attachment
*/
typedef struct _hogl_renderbuffer hogl_renderbuffer;

/**
 * @brief Attribute pointer description struct containing data about the attribute pointer 
*/
typedef struct _hogl_ap_desc {
	/**
	 * @brief Internal attribute pointer index from 0 to max amount of attributes
	*/
	int index;

	/**
	 * @brief Number of elements in the attribute pointer, cannot exceed 4 or be less than 1
	*/
	int ecount;

	/**
	 * @brief Attribute pointer element type
	*/
	hogl_element_type type;

	/**
	 * @brief Normalize data or not
	*/
	bool normalized;

	/**
	 * @brief Single ap entry size, this is the space inside a buffer for a single element for example passing a 4x4 matrix
	 * the stride will be sizeof(16 * sizeof(type)), for a standard float matrix this will be 16 * 4 = 64 bytes, this allows
	 * to store large amounts of matrices such as transformation matrices inside a single buffer
	*/
	size_t stride;

	/**
	 * @brief Number of bytes until start of data, just like with stride if for example we are passing a matrix this will be 
	 * a single row/column size, since the max amount for an attribute pointer in OpenGL is a single vec4 which is 16 bytes.
	 * the first offset will be 0 second one will be 16 third one 32 fourth one 48, so in total passing a matrix will use
	 * 4 attribute pointers
	*/
	size_t offset;

	/**
	 * @brief The attribute data divisor, usually this will be 0 for non instanced rendering 1 for indexed rendering
	*/
	int divisor;
} hogl_ap_desc;

/**
 * @brief Single VAO buffer description, containing data about a vbo should be initialized and used, a copy of this is
 * kept inside a VAO
*/
typedef struct _hogl_vbo_desc {
	hogl_vbo_type type;
	hogl_vbo_usage usage;

	hogl_ap_desc* ap_desc;
	size_t desc_size;

	/**
	 * @brief Size to allocate for a buffer, this can be changed later
	*/
	size_t data_size;

	/**
	 * @brief Initial data inside a buffer, if the usage is static this should
	 * contain the data and shouldn't change. If no data is present then this has to
	 * be NULL
	*/
	void* data;
} hogl_vbo_desc;

/**
 * @brief Shader object description, containing data needed to create a shader
*/
typedef struct _hogl_shader_desc {
	const char* vertex_source;
	const char* fragment_source;
} hogl_shader_desc;

/**
 * @brief Uniform buffer object description
*/
typedef struct _hogl_ubo_desc {
	/**
	 * @brief Binding point for the uniform buffer
	*/
	int bp;

	/**
	 * @brief The size of the internal ubo buffer
	*/
	size_t stride;

	/**
	 * @brief Offset for binding block
	*/
	size_t offset;
} hogl_ubo_desc;

/**
 * @brief Texture description
*/
typedef struct _hogl_texture_desc {
	/**
	 * @brief Wrapping type for the x axis
	*/
	hogl_wrap_type xwrap;

	/**
	 * @brief Wrapping type for the y axis
	*/
	hogl_wrap_type ywrap;

	/**
	 * @brief Wrapping type for the z axis
	*/
	hogl_wrap_type zwrap;

	/**
	 * @brief Magnification filter type, not all are valid for more check hogl_filter_type
	*/
	hogl_filter_type mag_filter;

	/**
	 * @brief Minification filter type
	*/
	hogl_filter_type min_filter;
} hogl_texture_desc;

/**
 * @brief Framebuffer description containing all the needed values for its attachments
*/
typedef struct _hogl_framebuffer_desc {
	/**
	 * @brief Width of the framebuffer, must match for all its attachments
	*/
	unsigned int width;

	/**
	 * @brief Height of the framebuffer, must match for all its attachments
	*/
	unsigned int height;

	/**
	 * @brief Color attachment of the framebuffer NULL if not needed
	*/
	hogl_texture** color_attachments;

	/**
	 * @brief Color attachment array size
	*/
	size_t ca_size;

	/**
	 * @brief Renderbuffer attachments
	*/
	hogl_renderbuffer** render_attachments;

	/**
	 * @brief Render attachment array size
	*/
	size_t ra_size;
} hogl_framebuffer_desc;

/**
 * @brief Texture data
*/
typedef struct _hogl_texture_data {
	/**
	 * @brief Format that will be displayed on the screen
	*/
	hogl_texture_format display_format;

	/**
	 * @brief Format of data elements
	*/
	hogl_texture_format data_format;

	hogl_element_type etype;

	unsigned int height;
	unsigned int width;

	void* data;
} hogl_texture_data;

/**
 * @brief Create a new vao with the specified description and store inside the pointer
 * @param vao Where to store the new vao
*/
HOGL_API void hogl_vao_new(hogl_vao** vao);

/**
 * @brief Binds the specified vao to the OpenGL state machine
 * @param vao VAO to bind
*/
HOGL_API void hogl_vao_bind(hogl_vao* vao);

/**
 * @brief Allocates an internal vertex buffer array for the vao, this function can only be called once, this means adding
 * more buffers later is not allowed. The newly created vertex buffers are stored as a range from 0 to size - 1
 * @param vao VAO to allocated buffers for
 * @param descs Array of vbo descriptions
 * @param size Size of types array
 * @return Returns error codes:
 *		HOGL_ERROR_NONE					if operation was successful
 *		HOGL_ERROR_ALREADY_ALLOCATED	if the vao buffers were already allocated
 *		HOGL_ERROR_OPENGL_GENERIC		if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_vao_alloc_buffers(hogl_vao* vao, hogl_vbo_desc* descs, size_t size);

/**
 * @brief Resizes the specified vbo inside a vao with the new size and optionally fills it with data,
 * keeps the usage and type the same
 * @param vao VAO where the vbo is
 * @param vbo Internal vbo index
 * @param size New buffer size
 * @param data Optional buffer data, NULL if none
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if the vbo parameter is larger than the allocated buffer size
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_vao_buffer_resize(hogl_vao* vao, int vbo, size_t size, void* data);

/**
 * @brief Sets the data of a vertex buffer that is stored inside a vertex array, before using this function make sure to
 * allocate buffers inside a vao first
 * @param vao Vertex array where the vertex buffer is being set
 * @param vbo Internal index of the vertex buffer
 * @param vbo_offset Offset int bytes from the start of the vbo internal buffer
 * @param data Pointer to the data that you want to set this will be bound checked if enabled
 * @param size Size of the data array pointer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if the vbo parameter is larger than the allocated buffer size or 
 *									if the vbo allocated size is less than what is being set
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_vao_buffer_data(hogl_vao* vao, int vbo, size_t vbo_offset, void* data, size_t size);

/**
 * @brief Frees all resources associated with the specified vao
 * @param vao VAO to free
*/
HOGL_API void hogl_vao_free(hogl_vao* vao);

/**
 * @brief Create a new buffer with the specified description and store inside the pointer
 * @param ubo Where to store the new ubo
 * @param desc Description of the new ubo
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if the stride is 0
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_ubo_new(hogl_ubo** ubo, hogl_ubo_desc desc);

/**
 * @brief Binds the specified ubo to the OpenGL state machine
 * @param ubo UBO to bind
*/
HOGL_API void hogl_ubo_bind(hogl_ubo* ubo);

/**
 * @brief Sets the specified ubo data
 * @param ubo Buffer whose data to set
 * @param data Data pointer, this will be bound checked if enabled
 * @param size Size of data
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_ubo_data(hogl_ubo* ubo, void* data, size_t size);

/**
 * @brief Frees all resources associated with the specified ubo
 * @param ubo UBO to free
*/
HOGL_API void hogl_ubo_free(hogl_ubo* ubo);

/**
 * @brief Create a new shader with the specified description and store inside the pointer
 * @param shader Where to store the new object
 * @param desc Description of the shader
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_SHADER_COMPILE	if one of the specified shaders failed to compile
 *		HOGL_ERROR_SHADER_LINK		if the program failed to link
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_shader_new(hogl_shader** shader, hogl_shader_desc desc);

/**
 * @brief Links the specified ubo to the specified shader bind point
 * @param shader Shader to link
 * @param ubo_name Name of the ubo in the shader
 * @param bp Bind point to set for the specified ubo_name buffer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNIFORM_UNKNOWN	if the specified name is not found in the shader
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_shader_ubo_binding(hogl_shader* shader, const char* ubo_name, unsigned int bp);

/**
 * @brief Links the specified sampler to the specified shader bind point
 * @param shader Shader to link
 * @param sample_name Name of the sampler variable
 * @param bp Bind point
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_UNIFORM_UNKNOWN	if the specified name is not found in the shader
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_shader_sampler_location(hogl_shader* shader, const char* sampler_name, unsigned int bp);

/**
 * @brief Binds the specified shader to the OpenGL state machine
 * @param shader Shader to bind
*/
HOGL_API void hogl_shader_bind(hogl_shader* shader);

/**
 * @brief Frees all resources associated with the specified shader
 * @param shader Shader to free
*/
HOGL_API void hogl_shader_free(hogl_shader* shader);

/**
 * @brief Create a new texture with the specified description and store inside the pointer
 * @param texture Where to store the new object
 * @param desc Description of the texture
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_BAD_ARGUMENT		if the specified mag filter is not allowed
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_texture_new(hogl_texture** texture, hogl_texture_desc desc);

/**
 * @brief Create a new cubemap texture with the specified description and store inside the pointer
 * @param cm Where to store the new object
 * @param desc Description of the texture
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
*		HOGL_ERROR_BAD_ARGUMENT		if the specified mag filter is not allowed
* 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_cm_new(hogl_texture** cm, hogl_texture_desc desc);

/**
 * @brief Binds the specified shader to the OpenGL state machine
 * @param texture Texture to bind
 * @param slot Texture slot to bind to
*/
HOGL_API void hogl_texture_bind(hogl_texture* texture, int slot);

/**
 * @brief Sets the active side of the cubemap
 * @param cm Cubemap to set
 * @param side Side to set as active
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_BAD_ARGUMENT		if the specified side is not valid
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_cm_active_side(hogl_texture* cm, hogl_cm_side side);

/**
 * @brief Sets the texture data, first call will also set all texture description parameters.
 * @param texture Texture object
 * @param data New texture data
*/
HOGL_API void hogl_set_texture_data(hogl_texture* texture, hogl_texture_data* data);

/**
 * @brief Generates mipmaps for the specified texture
 * @param texture Texture to generate minmap for
*/
HOGL_API void hogl_texture_gen_mipmap(hogl_texture* texture);

/**
 * @brief Frees all resources associated with the specified texture
 * @param texture Texture to free
*/
HOGL_API void hogl_texture_free(hogl_texture* texture);

/**
 * @brief Create a new framebuffer with the specified description and store inside the pointer
 * @param framebuffer Where to store the new object
 * @param desc Description of the framebuffer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_FBO_INCOMPLETE	if the framebuffer is incomplete, meaning something was wrong with attachments
 *		HOGL_ERROR_BAD_ARGUMENT		if more than 2 renderbuffers are passed
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_framebuffer_new(hogl_framebuffer** framebuffer, hogl_framebuffer_desc desc);

/**
 * @brief Binds the specified framebuffer to the OpenGL state machine
 * @param framebuffer Framebuffer to bind
*/
HOGL_API void hogl_framebuffer_bind(hogl_framebuffer* framebuffer);

/**
 * @brief Binds a texture to the specified framebuffer texture slot attachment
 * @param framebuffer Framebuffer to attach to
 * @param texture Texture to attach
 * @param slot Slot where the texture will be attached
 * @param mip Mipmap level default 0
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_FBO_INCOMPLETE	if the framebuffer is incomplete, meaning something was wrong with attachments
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_framebuffer_ca(hogl_framebuffer* framebuffer, hogl_texture* texture, unsigned int slot, unsigned int mip);

/**
 * @brief Frees all resources associated with the specified framebuffer
 * @param framebuffer Framebuffer to free
*/
HOGL_API void hogl_framebuffer_free(hogl_framebuffer* framebuffer);

/**
 * @brief Resets the framebuffer to the default
*/
HOGL_API void hogl_reset_framebuffer(void);

/**
 * @brief Create a new renderbuffer with the specified parameters and store inside the pointer
 * @param renderbuffer Where to store the new object
 * @param format Renderbuffer internal format
 * @param width Initial width of the render buffer
 * @param height Initial height of the render buffer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_BAD_ARGUMENT		if the format is invalid
 * 		HOGL_ERROR_OPENGL_GENERIC	if hogl_gl_check failed at any point
*/
HOGL_API hogl_error hogl_renderbuffer_new(hogl_renderbuffer** renderbuffer, hogl_rbuffer_format format, unsigned int width, unsigned int height);

/**
 * @brief Resizes the specified renderbuffer
 * @param renderbuffer Renderbuffer to resize
 * @param width New width of the renderbuffer
 * @param height New height of the renderbuffer
*/
HOGL_API void hogl_renderbuffer_resize(hogl_renderbuffer* renderbuffer, unsigned int width, unsigned int height);

/**
 * @brief Binds the renderbuffer to the OpenGL state machine
 * @param renderbuffer Renderbuffer to bind
*/
HOGL_API void hogl_renderbuffer_bind(hogl_renderbuffer* renderbuffer);

/**
 * @brief Frees all resources associated with the specified renderbuffer, make sure its not used by any framebuffer before destroying
 * @param renderbuffer Renderbuffer to free
*/
HOGL_API void hogl_renderbuffer_free(hogl_renderbuffer* renderbuffer);

#endif