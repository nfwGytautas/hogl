#include "hogl_gl_primitive.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_core/hogl_log.h"
#include "hogl_core/hogl_memory.h"

#define SHADER_LOG_LENGTH 512
#define MIN_FBO_COLOR_ATTACHMENT 8

typedef struct _hogl_vbo_meta {
	unsigned int type;
	unsigned int usage;

#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	size_t size;
#endif
	
#ifndef HOGL_DISABLE_GL_WARNING
	bool has_data;
#endif
} hogl_vbo_meta;

typedef struct _hogl_vao {
	unsigned int id;

	unsigned int* vbo_ids;
	hogl_vbo_meta* vbos;
	size_t vbo_count;
} hogl_vao;

typedef struct _hogl_ubo {
	unsigned int id;

#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	size_t stride;
#endif
} hogl_ubo;

typedef struct _hogl_shader {
	unsigned int id;
} hogl_shader;

typedef struct _hogl_texture {
	unsigned int id;
	unsigned int target;

	unsigned int cside;
} hogl_texture;

typedef struct _hogl_framebuffer {
	unsigned int id;
} hogl_framebuffer;

typedef struct _hogl_renderbuffer {
	unsigned int id;

	unsigned int format;
	unsigned int attachment_type;
} hogl_renderbuffer;

void __parse_vbo_from_desc(hogl_vbo_meta* vbo, hogl_vbo_desc* desc) {
	// Type
	switch (desc->type)
	{
		case HOGL_VBOT_ARRAY_BUFFER:
			vbo->type = GL_ARRAY_BUFFER;
			break;
		case HOGL_VBOT_ELEMENT_BUFFER:
			vbo->type = GL_ELEMENT_ARRAY_BUFFER;
			break;
		default:
			hogl_log_warn("No type specified for vbo, assuming ARRAY buffer");
			vbo->type = GL_ARRAY_BUFFER;
	}

	// Usage
	switch (desc->usage)
	{
		case HOGL_VBOU_STATIC:
			vbo->usage = GL_STATIC_DRAW;
			break;
		case HOGL_VBOU_DYNAMIC:
			vbo->usage = GL_DYNAMIC_DRAW;
			break;
		case HOGL_VBOU_STREAM:
			vbo->usage = GL_STREAM_DRAW;
			break;
		default:
			hogl_log_warn("No usage specified for vbo, assuming DYNAMIC buffer");
			vbo->usage = GL_DYNAMIC_DRAW;
	}

#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	vbo->size = desc->size;

	if (vbo->size == 0) {
		hogl_log_warn("Creating a 0 length vbo");
	}
#endif

#ifndef HOGL_DISABLE_GL_WARNING
	if (vbo->usage == GL_STATIC_DRAW && desc->data == NULL) {
		hogl_log_warn("Initializing STATIC vbo without any initial data");
	}

	if (desc->data != NULL) {
		vbo->has_data = true;
	}
#endif
}

unsigned int __get_type(hogl_element_type type) {
	switch (type) {
		case HOGL_ET_FLOAT:
			return GL_FLOAT;
		case HOGL_ET_UBYTE:
			return GL_UNSIGNED_BYTE;
		case HOGL_ET_UINT:
			return GL_UNSIGNED_INT;
		default:
			hogl_log_warn("No element type specified, assuming FLOAT");
			return GL_FLOAT;
	}
}

void __write_buffer_data(unsigned int target, size_t offset, void* data, size_t size) {
	void* dst = glMapBuffer(target, GL_WRITE_ONLY);
	hogl_memcpy((char*)dst + offset, data, size);
	glUnmapBuffer(target);
}

void __check_shader_status(unsigned int shader, unsigned int property, char** log) {
	int  success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		(*log) = (char*)hogl_malloc(SHADER_LOG_LENGTH * sizeof(char));
		glGetShaderInfoLog(shader, SHADER_LOG_LENGTH, NULL, (*log));
	}
}

unsigned int __parse_tformat(hogl_texture_format format) {
	switch (format)
	{
		case HOGL_TF_RED:
			return GL_RED;
		case HOGL_TF_RG:
			return GL_RG;
		case HOGL_TF_RGB:
			return GL_RGB;
		case HOGL_TF_RGBA:
			return GL_RGBA;
		case HOGL_TF_R16F:
			return GL_R16F;
		case HOGL_TF_RGB16F:
			return GL_RGB16F;
		case HOGL_TF_RGBA16F:
			return GL_RGBA16F;
		default:
			hogl_log_warn("No texture format specified, assuming RGB");
			return 0;
	}
}

unsigned int __parse_wrap(hogl_wrap_type type) {
	switch (type) {
	case HOGL_WT_REPEAT:
		return GL_REPEAT;
	case HOGL_WT_MIRRORED:
		return GL_MIRRORED_REPEAT;
	case HOGL_WT_EDGE_CLAMP:
		return GL_CLAMP_TO_EDGE;
	case HOGL_WT_BORDER_CLAMP:
		return GL_CLAMP_TO_BORDER;
	default:
		hogl_log_warn("No texture wrap specified, assuming REPEAT");
		return GL_REPEAT;
	}
}

unsigned int __parse_filter(hogl_filter_type type) {
	switch (type)
	{
	case HOGL_FT_NEAREST:
		return GL_NEAREST;
	case HOGL_FT_LINEAR:
		return GL_LINEAR;
	case HOGL_FT_NEAREST_MIPMAP_NEAREST:
		return GL_NEAREST_MIPMAP_NEAREST;
	case HOGL_FT_NEAREST_MIPMAP_LINEAR:
		return GL_NEAREST_MIPMAP_LINEAR;
	case HOGL_FT_LINEAR_MIPMAP_NEAREST:
		return GL_LINEAR_MIPMAP_NEAREST;
	case HOGL_FT_LINEAR_MIPMAP_LINEAR:
		return GL_LINEAR_MIPMAP_NEAREST;
	default:
		hogl_log_warn("No texture filter specified, assuming NEAREST");
		return GL_NEAREST;
	}
}

hogl_error __parse_texture_from_desc(hogl_texture* texture, hogl_texture_desc* desc) {
	// Wrapping
	if (desc->xwrap != HOGL_WT_DONT_WRAP) {
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, __parse_wrap(desc->xwrap));
	}

	if (desc->ywrap != HOGL_WT_DONT_WRAP) {
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, __parse_wrap(desc->ywrap));
	}

	if (desc->zwrap != HOGL_WT_DONT_WRAP) {
		glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, __parse_wrap(desc->zwrap));
	}

	// Mag filter
	if (desc->mag_filter != HOGL_FT_NONE) {
#ifndef HOGL_DISABLE_GL_WARNING
		if (desc->mag_filter != HOGL_FT_NEAREST &&
			desc->mag_filter != HOGL_FT_LINEAR) {
			hogl_log_error("Mag filter can only be either NEAREST or LINEAR");
			return HOGL_ERROR_BAD_ARGUMENT;
		}
#endif
		glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, __parse_filter(desc->mag_filter));
	}

	// Min filter
	if (desc->min_filter != HOGL_FT_NONE) {
		glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, __parse_filter(desc->min_filter));
	}

	return HOGL_ERROR_NONE;
}

unsigned int __parse_rbuffer(hogl_rbuffer_format format) {
	switch (format) {
		case HOGL_RBF_d16:
			return GL_DEPTH_COMPONENT16;
		case HOGL_RBF_d24:
			return GL_DEPTH_COMPONENT24;
		case HOGL_RBF_d32F:
			return GL_DEPTH_COMPONENT32F;
		case HOGL_RBF_d24_s8:
			return GL_DEPTH24_STENCIL8;
		case HOGL_RBF_d32F_s8:
			return GL_DEPTH32F_STENCIL8;
		case HOGL_RBF_si8:
			return GL_STENCIL_INDEX8;
		default:
			hogl_log_warn("No texture filter specified, assuming d24_s8");
			return GL_DEPTH24_STENCIL8;
	}
}

void hogl_vao_new(hogl_vao** vao)
{
	(*vao) = (hogl_vao*)hogl_malloc(sizeof(hogl_vao));
	glGenVertexArrays(1, &(*vao)->id);
}

void hogl_vao_bind(hogl_vao* vao)
{
#ifndef HOGL_DISABLE_GL_WARNING
	if (vao->vbo_count == 0) {
		hogl_log_warn("Binding vao without any vertex buffers");
	}
	else {
		for (size_t i = 0; i < vao->vbo_count; i++) {
			if (!vao->vbos[i].has_data) {
				hogl_log_warn("Binding vao with vbo %d not having any data", i);
			}
		}
	}
#endif

	glBindVertexArray(vao->id);
}

void hogl_vao_alloc_buffers(hogl_vao* vao, hogl_vbo_desc** descs, size_t size)
{
	glBindVertexArray(vao->id);

	// Allocate a vbo buffer
	vao->vbos = (hogl_vbo_meta*)hogl_malloc(sizeof(hogl_vbo_meta) * size);
	vao->vbo_count = size;

	vao->vbo_ids = (unsigned int*)hogl_malloc(sizeof(unsigned int) * size);
	glGenBuffers(size, vao->vbo_ids);

	for (size_t i = 0; i < size; i++) {
		__parse_vbo_from_desc(&vao->vbos[i], descs[i]);

		// Bind and preallocate buffer
		glBindBuffer(vao->vbos[i].type, vao->vbo_ids[i]);
		glBufferData(vao->vbos[i].type, descs[i]->size, descs[i]->data, vao->vbos[i].usage);

		// Configure ap
		glEnableVertexAttribArray(descs[i]->ap_desc.index);
		glVertexAttribPointer(
			descs[i]->ap_desc.index, 
			descs[i]->ap_desc.ecount,
			__get_type(descs[i]->ap_desc.type),
			descs[i]->ap_desc.normalized,
			descs[i]->ap_desc.stride,
			(void*)descs[i]->ap_desc.offset);
		glVertexAttribDivisor(descs[i]->ap_desc.index, descs[i]->ap_desc.divisor);
	}
}

hogl_error hogl_vao_buffer_resize(hogl_vao* vao, int vbo, size_t size, void* data)
{
#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	if (vao->vbo_count <= vbo) {
		hogl_log_error("Trying to access vbo %d while the vao only has %d", vbo, vao->vbo_count);
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (size == 0) {
		hogl_log_error("Trying to resize to a 0 size buffer, aborting");
		return HOGL_ERROR_OUT_OF_RANGE;
	}
#endif

#ifndef HOGL_DISABLE_GL_WARNING
	// Check if not static
	if (vao->vbos[vbo].usage == GL_STATIC_DRAW) {
		hogl_log_warn("Resizing a static buffer");
	}
#endif

	glBindVertexArray(vao->id);
	glBindBuffer(vao->vbos[vbo].type, vao->vbo_ids[vbo]);
	glBufferData(vao->vbos[vbo].type, size, data, vao->vbos[vbo].usage);

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vao_buffer_data(hogl_vao* vao, int vbo, size_t vbo_offset, void* data, size_t size)
{
#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	if (vao->vbo_count <= vbo) {
		hogl_log_error("Trying to access vbo %d while the vao only has %d", vbo, vao->vbo_count);
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (vao->vbos[vbo].size < vbo_offset + size) {
		hogl_log_error("Trying to write %ld length data to a buffer whose length is only %ld", vbo_offset + size, vao->vbos[vbo].size);
		return HOGL_ERROR_OUT_OF_RANGE;
	}
#endif

#ifndef HOGL_DISABLE_GL_WARNING
	// Check if not static
	if (vao->vbos[vbo].usage == GL_STATIC_DRAW) {
		hogl_log_warn("Writing to a static buffer");
	}
#endif

	glBindVertexArray(vao->id);
	glBindBuffer(vao->vbos[vbo].type, vao->vbo_ids[vbo]);
	__write_buffer_data(vao->vbos[vbo].type, vbo_offset, data, size);

#ifndef HOGL_DISABLE_GL_WARNING
	vao->vbos[vbo].has_data = true;
#endif

	return HOGL_ERROR_NONE;
}

void hogl_vao_free(hogl_vao* vao)
{
	glDeleteBuffers(vao->vbo_count, vao->vbo_ids);
	glDeleteVertexArrays(1, &vao->id);
	hogl_free(vao->vbos);
	hogl_free(vao);
}

hogl_error hogl_ubo_new(hogl_ubo** ubo, hogl_ubo_desc desc)
{
#ifndef HOGL_DISABLE_GL_WARNING
	if (desc.stride % 4 != 0) {
		hogl_log_warn("%ld is not divisible by modulo 4, which can lead to alignment issues");
	}
#endif

#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	if (desc.stride == 0) {
		hogl_log_error("Trying to access allocate 0 size ubo, aborting");
		return HOGL_ERROR_OUT_OF_RANGE;
	}
#endif

	(*ubo) = (hogl_ubo*)hogl_malloc(sizeof(hogl_ubo));
	glGenBuffers(1, &(*ubo)->id);
	glBindBuffer(GL_UNIFORM_BUFFER, (*ubo)->id);
	glBufferData(GL_UNIFORM_BUFFER, desc.stride, NULL, GL_DYNAMIC_DRAW);

	(*ubo)->stride = desc.stride;

	glBindBufferRange(GL_UNIFORM_BUFFER, desc.bp, (*ubo)->id, desc.offset, desc.stride);

	return HOGL_ERROR_NONE;
}

void hogl_ubo_bind(hogl_ubo* ubo)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo->id);
}

hogl_error hogl_ubo_data(hogl_ubo* ubo, void* data, size_t size)
{
#ifndef HOGL_DISABLE_GL_BOUND_CHECKING
	if (ubo->stride < size) {
		hogl_log_error("Trying to write %ld length data to a buffer whose length is only %ld", size, ubo->stride);
		return HOGL_ERROR_OUT_OF_RANGE;
	}
#endif

	glBindBuffer(GL_UNIFORM_BUFFER, ubo->id);
	__write_buffer_data(GL_UNIFORM_BUFFER, 0, data, size);

	return HOGL_ERROR_NONE;
}

void hogl_ubo_free(hogl_ubo* ubo)
{
	glDeleteBuffers(1, &ubo->id);
	hogl_free(ubo);
}

hogl_error hogl_shader_new(hogl_shader** shader, hogl_shader_desc desc)
{
	char* log = NULL;
	unsigned int program = 0;
	unsigned int vertex_shader = 0;
	unsigned int fragment_shader = 0;

	// Vertex
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &desc.vertex_source, NULL);
	glCompileShader(vertex_shader);

#ifndef HOGL_DISABLE_GL_WARNING
	__check_shader_status(vertex_shader, GL_COMPILE_STATUS, &log);
	if (log != NULL) {
		glDeleteShader(vertex_shader);
		hogl_free(log);
		return HOGL_ERROR_SHADER_COMPILE;
	}
#endif

	// Fragment
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &desc.fragment_source, NULL);
	glCompileShader(fragment_shader);

#ifndef HOGL_DISABLE_GL_WARNING
	__check_shader_status(fragment_shader, GL_COMPILE_STATUS, &log);
	if (log != NULL) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		hogl_free(log);
		return HOGL_ERROR_SHADER_COMPILE;
	}
#endif

	// Link
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, vertex_shader);
	glLinkProgram(program);

#ifndef HOGL_DISABLE_GL_WARNING
	__check_shader_status(program, GL_LINK_STATUS, &log);
	if (log != NULL) {
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(program);
		hogl_free(log);
		return HOGL_ERROR_SHADER_LINK;
	}
#endif

	(*shader) = (hogl_shader*)hogl_malloc(sizeof(hogl_shader));
	(*shader)->id = program;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return HOGL_ERROR_NONE;
}

void hogl_shader_ubo_binding(hogl_shader* shader, const char* ubo_name, unsigned int bp)
{
	unsigned int index = glGetUniformBlockIndex(shader->id, ubo_name);
	glUniformBlockBinding(shader->id, index, bp);
}

void hogl_shader_bind(hogl_shader* shader)
{
	glUseProgram(shader->id);
}

void hogl_shader_free(hogl_shader* shader)
{
	glDeleteProgram(shader->id);
	hogl_free(shader);
}

hogl_error hogl_texture_new(hogl_texture** texture, hogl_texture_desc desc)
{
	(*texture) = (hogl_texture*)hogl_malloc(sizeof(hogl_texture));
	glGenTextures(1, &(*texture)->id);
	(*texture)->target = GL_TEXTURE_2D;
	glBindTexture(GL_TEXTURE_2D, (*texture)->id);
	(*texture)->cside = GL_TEXTURE_2D;
	return __parse_texture_from_desc(*texture, &desc);
}

hogl_error hogl_cm_new(hogl_texture** cm, hogl_texture_desc desc)
{
	(*cm) = (hogl_texture*)hogl_malloc(sizeof(hogl_texture));
	glGenTextures(1, &(*cm)->id);
	(*cm)->target = GL_TEXTURE_CUBE_MAP;
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*cm)->id);
	(*cm)->cside = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	return __parse_texture_from_desc(*cm, &desc);
}

void hogl_texture_bind(hogl_texture* texture, int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(texture->target, texture->id);
}

hogl_error hogl_cm_active_side(hogl_texture* cm, hogl_cm_side side)
{
	switch (side) {
	case HOGL_CMS_RIGHT:
		cm->cside = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
		return HOGL_ERROR_NONE;
	case HOGL_CMS_LEFT:
		cm->cside = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
		return HOGL_ERROR_NONE;
	case HOGL_CMS_TOP:
		cm->cside = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
		return HOGL_ERROR_NONE;
	case HOGL_CMS_BOTTOM:
		cm->cside = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
		return HOGL_ERROR_NONE;
	case HOGL_CMS_BACK:
		cm->cside = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
		return HOGL_ERROR_NONE;
	case HOGL_CMS_FRONT:
		cm->cside = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
		return HOGL_ERROR_NONE;
	default:
		hogl_log_error("Non valid cube map side passed defaulting to RIGHT")
		return HOGL_ERROR_BAD_ARGUMENT;
	}
}

void hogl_set_texture_data(hogl_texture* texture, hogl_texture_data* data)
{
	glBindTexture(texture->target, texture->id);
	glTexImage2D(
		texture->cside, 
		0, 
		__parse_tformat(data->data_format), 
		data->width, 
		data->height, 
		0,
		__parse_tformat(data->display_format), 
		__get_type(data->etype), 
		data->data);
}

void hogl_texture_gen_mipmap(hogl_texture* texture)
{
	glBindTexture(texture->target, texture->id);
	glGenerateMipmap(texture->target);
}

void hogl_texture_free(hogl_texture* texture)
{
	glDeleteTextures(1, &texture->id);
	hogl_free(texture);
}

hogl_error hogl_framebuffer_new(hogl_framebuffer** framebuffer, hogl_framebuffer_desc desc)
{
	(*framebuffer) = (hogl_framebuffer*)hogl_malloc(sizeof(hogl_framebuffer));
	glGenFramebuffers(1, &(*framebuffer)->id);
	glBindFramebuffer(GL_FRAMEBUFFER, (*framebuffer)->id);

#ifndef HOGL_DISABLE_GL_WARNING
	if (desc.ca_size > MIN_FBO_COLOR_ATTACHMENT) {
		hogl_log_warn("Attaching more color attachments %ld then the standard minimum %ld", desc.ca_size, MIN_FBO_COLOR_ATTACHMENT);
	}

	if (desc.ra_size > 2) {
		hogl_log_error("More than 2 render buffers supplied");
		return HOGL_ERROR_BAD_ARGUMENT;
	}
#endif

	// Color attachments
	for (size_t i = 0; i < desc.ca_size; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, desc.color_attachments[i]->target, desc.color_attachments[i]->id, 0);
	}

	// Render attachments
	for (size_t i = 0; i < desc.ra_size; i++) {
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, desc.render_attachments[i]->attachment_type, GL_RENDERBUFFER, desc.render_attachments[i]->id);
	}

	// Validate
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		hogl_log_error("Framebuffer incomplete");
		return HOGL_ERROR_FBO_INCOMPLETE;
	}

	hogl_reset_framebuffer();

	return HOGL_ERROR_NONE;
}

void hogl_framebuffer_bind(hogl_framebuffer* framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
}

void hogl_framebuffer_free(hogl_framebuffer* framebuffer)
{
	glDeleteFramebuffers(1, &framebuffer->id);
	hogl_free(framebuffer);
}

void hogl_reset_framebuffer(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

hogl_error hogl_renderbuffer_new(hogl_renderbuffer** renderbuffer, hogl_rbuffer_format format, unsigned int width, unsigned int height)
{
	(*renderbuffer) = (hogl_renderbuffer*)hogl_malloc(sizeof(hogl_renderbuffer));
	glGenRenderbuffers(1, &(*renderbuffer)->id);
	glBindRenderbuffer(GL_RENDERBUFFER, (*renderbuffer)->id);

	(*renderbuffer)->format = __parse_rbuffer(format);
	
	switch (format) {
	case HOGL_RBF_d16:
	case HOGL_RBF_d24:
	case HOGL_RBF_d32F:
		(*renderbuffer)->attachment_type = GL_DEPTH_ATTACHMENT;
		break;

	case HOGL_RBF_d24_s8:
	case HOGL_RBF_d32F_s8:
		(*renderbuffer)->attachment_type = GL_DEPTH_STENCIL_ATTACHMENT;
		break;

	case HOGL_RBF_si8:
		(*renderbuffer)->attachment_type = GL_STENCIL_ATTACHMENT;
		break;

	default:
		hogl_log_error("Bad renderbuffer attachment format");
		return HOGL_ERROR_BAD_ARGUMENT;
	}

	glRenderbufferStorage(GL_RENDERBUFFER, (*renderbuffer)->format, width, height);

	return HOGL_ERROR_NONE;
}

void hogl_renderbuffer_resize(hogl_renderbuffer* renderbuffer, unsigned int width, unsigned int height)
{
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->id);
	glRenderbufferStorage(GL_RENDERBUFFER, renderbuffer->format, width, height);
}

void hogl_renderbuffer_bind(hogl_renderbuffer* renderbuffer)
{
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer->id);
}

void hogl_renderbuffer_free(hogl_renderbuffer* renderbuffer)
{
	glDeleteRenderbuffers(1, &renderbuffer->id);
	hogl_free(renderbuffer);
}
