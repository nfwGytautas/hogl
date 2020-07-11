#include "hogl_builder.hpp"

#include <iostream>
#include <algorithm>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_object.hpp"
#include "hogl_loader.hpp"

HOGL_NSPACE_BEGIN

GLenum get_type(hogl_vbo_type type)
{
    switch (type)
    {
    case hogl_vbo_type::ARRAY_BUFFER:
        return GL_ARRAY_BUFFER;
    case hogl_vbo_type::ELEMENT_BUFFER:
        return GL_ELEMENT_ARRAY_BUFFER;
    case hogl_vbo_type::UNSET:
        HOGL_LOG_WARN("Trying to get_type of a UNSET hogl_vbo_type");
        return 0;
    }

    return 0;
}

GLenum get_type_element(hogl_ap_type type)
{
    switch (type)
    {
    case hogl_ap_type::FLOAT:
        return GL_FLOAT;
    case hogl_ap_type::UINT:
        return GL_UNSIGNED_INT;
    }

    return 0;
}

GLenum get_usage(hogl_vbo_usage usage)
{
    switch (usage)
    {
    case hogl_vbo_usage::STREAM:
        return GL_STREAM_DRAW;
    case hogl_vbo_usage::STATIC:
        return GL_STATIC_DRAW;
    case hogl_vbo_usage::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    case hogl_vbo_usage::UNSET:
        HOGL_LOG_WARN("Trying to get_usage of a UNSET hogl_vbo_usage");
        return 0;
    }

    return 0;
}

void verify_shader_compilation(unsigned int shader)
{
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        HOGL_LOG_ERROR("Compilation failed with error code:\n" << infoLog << "\n");
    }
}

void verify_shader_link(unsigned int shader)
{
    int  success;
    char infoLog[512];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        HOGL_LOG_ERROR("Link failed with error code:\n" << infoLog << "\n");
    }
}

hogl_bldr_mesh::hogl_bldr_mesh(hogl_mesh* mesh)
    : m_mesh(mesh)
{
    if (m_mesh == nullptr)
    {
        HOGL_LOG_WARN("nullptr hogl_mesh passed to a builder");
        return;
    }

    glGenVertexArrays(1, &m_mesh->vao_id);
    glBindVertexArray(m_mesh->vao_id);
}

hogl_bldr_mesh& hogl_bldr_mesh::set_vbo(hogl_vbo_type type, unsigned int id)
{
    if (std::find(m_mesh->vbo_ids.begin(), m_mesh->vbo_ids.end(), id) != m_mesh->vbo_ids.end())
    {
        m_type = type;
        unsigned int gl_type = get_type(m_type);

        if (gl_type != 0)
        {
            glBindBuffer(gl_type, id);
            m_hasVBO = true;

            if (type == hogl_vbo_type::ELEMENT_BUFFER)
            {
                m_mesh->ebo_id = id;
            }
        }
    }
    else
    {
        HOGL_LOG_WARN("Setting vbo that is not associated with the specified object");
    }

    return *this;
}

hogl_bldr_mesh& hogl_bldr_mesh::add_vbo(hogl_vbo_type type)
{
    m_type = type;
    unsigned int gl_type = get_type(m_type);

    if (gl_type != 0)
    {
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        m_mesh->vbo_ids.push_back(vbo);
        glBindBuffer(gl_type, vbo);
        m_hasVBO = true;

        if (type == hogl_vbo_type::ELEMENT_BUFFER)
        {
            m_mesh->ebo_id = vbo;
        }
    }

    return *this;
}

hogl_bldr_mesh& hogl_bldr_mesh::add_vbo_data(hogl_vbo_usage usage, size_t size, void* data)
{
    if (!m_hasVBO)
    {
        HOGL_LOG_WARN("No VBO specified for add_vbo_data");
        return *this;
    }

    unsigned int gl_type = get_type(m_type);
    unsigned int gl_usage = get_usage(usage);

    if (gl_usage != 0 && gl_type != 0)
    {
        glBufferData(gl_type, size, data, gl_usage);

        if (m_type == hogl_vbo_type::ELEMENT_BUFFER)
        {
            m_mesh->vertice_count = size;
        }
    }

    return *this;
}

hogl_bldr_mesh& hogl_bldr_mesh::set_vbo_ap(unsigned int slot, hogl_ap_type etype, hogl_ap_count count, unsigned int stride, unsigned int offset)
{
    if (!m_hasVBO)
    {
        HOGL_LOG_WARN("No VBO specified for set_vbo_ap");
        return *this;
    }

    unsigned gl_type = get_type_element(etype);

    glVertexAttribPointer(slot, (GLint)count, gl_type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(slot);

    return *this;
}

hogl_bldr_mesh& hogl_bldr_mesh::set_vertices_count(unsigned int count)
{
    m_mesh->vertice_count = count;
    return *this;
}

hogl_mesh* hogl_bldr_mesh::ptr()
{
    return m_mesh;
}

hogl_bldr_shader::hogl_bldr_shader(hogl_shader* shader)
    : m_shader(shader)
{
    if (m_shader == nullptr)
    {
        HOGL_LOG_WARN("nullptr hogl_shader passed to a builder");
        return;
    }
}

hogl_bldr_shader& hogl_bldr_shader::set_shader(unsigned int shader)
{
    m_shader->shader_id = shader;
    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::add_shader()
{
    m_shader->shader_id = glCreateProgram();
    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::set_vertex_shader(unsigned int shader)
{
    m_tempVert = shader;
    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::add_vertex_source(const char* source)
{
    // Create shader, set its source and the compile it
    m_tempVert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(m_tempVert, 1, &source, NULL);
    glCompileShader(m_tempVert);

    // Print information if compilation failed
    verify_shader_compilation(m_tempVert);

    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::set_fragment_shader(unsigned int shader)
{
    m_tempFrag = shader;
    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::add_fragment_source(const char* source)
{
    // Create shader, set its source and the compile it
    m_tempFrag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(m_tempFrag, 1, &source, NULL);
    glCompileShader(m_tempFrag);

    // Print information if compilation failed
    verify_shader_compilation(m_tempFrag);

    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::link_shader()
{
    if (m_shader->shader_id == 0)
    {
        HOGL_LOG_WARN("Shader for object is not yet created");
        return *this;
    }

    if (m_tempVert == 0)
    {
        HOGL_LOG_WARN("Vertex shader either failed to be compiled or has not been created");
        return *this;
    }

    if (m_tempFrag == 0)
    {
        HOGL_LOG_WARN("Fragment shader either failed to be compiled or has not been created");
        return *this;
    }

    // Bind, attach and link
    glAttachShader(m_shader->shader_id, m_tempVert);
    glAttachShader(m_shader->shader_id, m_tempFrag);
    glLinkProgram(m_shader->shader_id);

    // Print link problems
    verify_shader_link(m_shader->shader_id);

    // Now delete fragment and vertex shaders
    glDeleteShader(m_tempVert);
    glDeleteShader(m_tempFrag);

    m_tempVert = 0;
    m_tempFrag = 0;

    m_hasShader = true;

    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::ubo_layout(const hogl_shader_layout& layout)
{
    if (!m_hasShader)
    {
        HOGL_LOG_ERROR("Cannot apply ubo layout to a non existent shader");
        return *this;
    }

    // Bind the shader
    glUseProgram(this->m_shader->shader_id);

    for (auto& block : layout.entries)
    {
        // Get block binding index
        unsigned int index = glGetUniformBlockIndex(this->m_shader->shader_id, block.first.c_str());

        // Check if this shader has a the specified ubo
        if (index != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(this->m_shader->shader_id, index, block.second);
        }
    }

    return *this;
}

hogl_bldr_shader& hogl_bldr_shader::sampler_layout(const hogl_shader_layout& layout)
{
    if (!m_hasShader)
    {
        HOGL_LOG_ERROR("Cannot apply ubo layout to a non existent shader");
        return *this;
    }

    // Bind the shader
    glUseProgram(this->m_shader->shader_id);

    for (auto& entry : layout.entries)
    {
        unsigned int location = glGetUniformLocation(m_shader->shader_id, entry.first.c_str());

        if (location != -1)
        {
            glUniform1i(location, entry.second);
        }
    }

    return *this;
}

hogl_shader* hogl_bldr_shader::ptr()
{
    return m_shader;
}

hogl_bldr_ubo::hogl_bldr_ubo(hogl_ubo* ubo)
    : m_ubo(ubo)
{
}

hogl_bldr_ubo& hogl_bldr_ubo::set_ubo(unsigned int ubo)
{
    m_ubo->ubo_id = ubo;
    return *this;
}

hogl_bldr_ubo& hogl_bldr_ubo::add_ubo()
{
    glGenBuffers(1, &m_ubo->ubo_id);
    return *this;
}

hogl_bldr_ubo& hogl_bldr_ubo::allocate(size_t size)
{
    m_ubo->allocated = size;
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo->ubo_id);
    glBufferData(GL_UNIFORM_BUFFER, m_ubo->allocated, NULL, GL_STATIC_DRAW);
    return *this;
}

hogl_bldr_ubo& hogl_bldr_ubo::add_range(unsigned int block, size_t offset, size_t size)
{
    if (offset + size > m_ubo->allocated)
    {
        HOGL_LOG_ERROR("Cannot specify a UBO bound range that is bigger than the allocated size");
        return *this;
    }
    m_ubo->bind_ranges.push_back({block, offset, size});
    glBindBufferRange(GL_UNIFORM_BUFFER, block, m_ubo->ubo_id, offset, size);
    return *this;
}

hogl_ubo* hogl_bldr_ubo::ptr()
{
    return m_ubo;
}

hogl_bldr_texture::hogl_bldr_texture(hogl_texture* texture)
    : m_texture(texture)
{
}

hogl_bldr_texture& hogl_bldr_texture::set_texture(unsigned int texture)
{
    m_texture->texture_id = texture;
    m_hasTexture = true;
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::add_texture()
{
    glGenTextures(1, &m_texture->texture_id);
    m_hasTexture = true;
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::add_data(hogl_loader_image* image_data)
{
    if (!m_hasTexture)
    {
        HOGL_LOG_ERROR("Cannot add data to non existant texture");
        return *this;
    }

    // Bind texture
    glBindTexture(GL_TEXTURE_2D, m_texture->texture_id);

    GLenum format = 0;
    switch (image_data->format)
    {
    case hogl_texture_format::RED:
        format = GL_RED;
        break;
    case hogl_texture_format::RGB:
        format = GL_RGB;
        break;
    case hogl_texture_format::RGBA:
        format = GL_RGBA;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, image_data->width, image_data->height, 0, format, GL_UNSIGNED_BYTE, image_data->data.get());
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::generate_mipmap()
{
    glGenerateMipmap(GL_TEXTURE_2D);
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::set_wrap(hogl_wrap_axis axis, hogl_wrap_type type)
{
    GLenum glaxis = 0;
    GLenum gltype = 0;

    switch (axis)
    {
    case hogl_wrap_axis::X:
        glaxis = GL_TEXTURE_WRAP_S;
        break;
    case hogl_wrap_axis::Y:
        glaxis = GL_TEXTURE_WRAP_T;
        break;
    case hogl_wrap_axis::Z:
        glaxis = GL_TEXTURE_WRAP_R;
        break;
    }

    switch (type)
    {
    case hogl_wrap_type::REPEAT:
        gltype = GL_REPEAT;
        break;
    case hogl_wrap_type::MIRRORED:
        gltype = GL_MIRRORED_REPEAT;
        break;
    case hogl_wrap_type::EDGE_CLAMP:
        gltype = GL_CLAMP_TO_EDGE;
        break;
    case hogl_wrap_type::BORDER_CLAMP:
        gltype = GL_CLAMP_TO_BORDER;
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, glaxis, gltype);
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::set_mag_filter(hogl_filter_type type)
{
    GLenum gltype = 0;

    switch (type)
    {
    case hogl_filter_type::NEAREST:
        gltype = GL_NEAREST;
        break;
    case hogl_filter_type::LINEAR:
        gltype = GL_LINEAR;
        break;
    case hogl_filter_type::NEAREST_MIPMAP_NEAREST:
        HOGL_LOG_WARN("Nearest mipmap nearest option is not valid for magnification filter");
        return *this;
    case hogl_filter_type::NEAREST_MIPMAP_LINEAR:
        HOGL_LOG_WARN("Nearest mipmap linear option is not valid for magnification filter");
        return *this;
    case hogl_filter_type::LINEAR_MIPMAP_NEAREST:
        HOGL_LOG_WARN("Linear mipmap nearest option is not valid for magnification filter");
        return *this;
    case hogl_filter_type::LINEAR_MIPMAP_LINEAR:
        HOGL_LOG_WARN("Linear mipmap linear option is not valid for magnification filter");
        return *this;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gltype);
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::set_min_filter(hogl_filter_type type)
{
    GLenum gltype = 0;

    switch (type)
    {
    case hogl_filter_type::NEAREST:
        gltype = GL_NEAREST;
        break;
    case hogl_filter_type::LINEAR:
        gltype = GL_LINEAR;
        break;
    case hogl_filter_type::NEAREST_MIPMAP_NEAREST:
        gltype = GL_NEAREST_MIPMAP_NEAREST;
        break;
    case hogl_filter_type::NEAREST_MIPMAP_LINEAR:
        gltype = GL_NEAREST_MIPMAP_LINEAR;
        break;
    case hogl_filter_type::LINEAR_MIPMAP_NEAREST:
        gltype = GL_LINEAR_MIPMAP_NEAREST;
        break;
    case hogl_filter_type::LINEAR_MIPMAP_LINEAR:
        gltype = GL_LINEAR_MIPMAP_LINEAR;
        break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gltype);
    return *this;
}

hogl_bldr_texture& hogl_bldr_texture::slot(unsigned int slot)
{
    if (slot < 0 && slot > HOGL_MAX_TEXTURE_SLOTS)
    {
        HOGL_LOG_WARN("Specified texture slot " << slot << " is either larger than " << HOGL_MAX_TEXTURE_SLOTS << " or is smaller than 0");
        return *this;
    }

    m_texture->slot = slot;
    return *this;
}

hogl_texture* hogl_bldr_texture::ptr()
{
    return m_texture;
}

hogl_bldr_mesh hogl_new_mesh()
{
    hogl_mesh* mesh = new hogl_mesh();
    return hogl_bldr_mesh(mesh);
}

hogl_bldr_shader hogl_new_shader()
{
    hogl_shader* shader = new hogl_shader();
    return hogl_bldr_shader(shader);
}

hogl_bldr_ubo hogl_new_ubo()
{
    hogl_ubo* ubo = new hogl_ubo();
    return hogl_bldr_ubo(ubo);
}

hogl_bldr_texture hogl_new_texture()
{
    hogl_texture* texture = new hogl_texture();
    return hogl_bldr_texture(texture);
}

void hogl_free_mesh(hogl_mesh*& mesh)
{
    // Free OpenGL resources here
    glDeleteVertexArrays(1, &mesh->vao_id);
    glDeleteBuffers(mesh->vbo_ids.size(), mesh->vbo_ids.data());
    glDeleteBuffers(1, &mesh->ebo_id);

    // Delete pointer
    delete mesh;
    mesh = nullptr;
}

void hogl_free_shader(hogl_shader*& shader)
{
    // Free OpenGL resource here
    glDeleteProgram(shader->shader_id);

    // Delete pointer
    delete shader;
    shader = nullptr;
}

void hogl_free_ubo(hogl_ubo*& ubo)
{
    glDeleteBuffers(1, &ubo->ubo_id);

    // Delete pointer
    delete ubo;
    ubo = nullptr;
}

void hogl_free_texture(hogl_texture*& texture)
{
    glDeleteTextures(1, &texture->texture_id);

    delete texture;
    texture = nullptr;
}

HOGL_NSPACE_END
