#include "hogl_builder.hpp"

#include <iostream>
#include <algorithm>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_object.hpp"

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
        std::cout << "[hogl:warn] Trying to get_type of a UNSET hogl_vbo_type\n";
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
        std::cout << "[hogl:warn] Trying to get_usage of a UNSET hogl_vbo_usage\n";
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
        std::cout << "[holg:error] Compilation failed with error code:\n" << infoLog << "\n";
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
        std::cout << "[holg:error] Link failed with error code:\n" << infoLog << "\n";
    }
}

hogl_obj_builder::hogl_obj_builder(hogl_object* object)
    : m_object(object)
{
    if (m_object == nullptr)
    {
        std::cout << "[hogl:warn] nullptr hogl_object passed to a builder\n";
    }
}

hogl_obj_builder& hogl_obj_builder::init()
{
    glGenVertexArrays(1, &m_object->vao_id);
    glBindVertexArray(m_object->vao_id);
    return *this;
}

hogl_obj_builder& hogl_obj_builder::set_vbo(hogl_vbo_type type, unsigned int id)
{
    if (std::find(m_object->vbo_ids.begin(), m_object->vbo_ids.end(), id) != m_object->vbo_ids.end())
    {
        m_type = type;
        unsigned int gl_type = get_type(m_type);

        if (gl_type != 0)
        {
            glBindBuffer(gl_type, id);
            m_hasVBO = true;

            if (type == hogl_vbo_type::ELEMENT_BUFFER)
            {
                m_object->ebo_id = id;
            }
        }
    }
    else
    {
        std::cout << "[hogl:warn] Setting vbo that is not associated with the specified object\n";
    }

    return *this;
}

hogl_obj_builder& hogl_obj_builder::add_vbo(hogl_vbo_type type)
{
    m_type = type;
    unsigned int gl_type = get_type(m_type);

    if (gl_type != 0)
    {
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        m_object->vbo_ids.push_back(vbo);
        glBindBuffer(gl_type, vbo);
        m_hasVBO = true;

        if (type == hogl_vbo_type::ELEMENT_BUFFER)
        {
            m_object->ebo_id = vbo;
        }
    }

    return *this;
}

hogl_obj_builder& hogl_obj_builder::add_vbo_data(hogl_vbo_usage usage, size_t size, void* data)
{
    if (!m_hasVBO)
    {
        std::cout << "[hogl:warn] No VBO specified for add_vbo_data\n";
        return *this;
    }

    unsigned int gl_type = get_type(m_type);
    unsigned int gl_usage = get_usage(usage);

    if (gl_usage != 0 && gl_type != 0)
    {
        glBufferData(gl_type, size, data, gl_usage);

        if (m_type == hogl_vbo_type::ELEMENT_BUFFER)
        {
            m_object->vertice_count = size;
        }
    }

    return *this;
}

hogl_obj_builder& hogl_obj_builder::set_vbo_ap(unsigned int slot, hogl_ap_type etype, hogl_ap_count count, unsigned int stride, unsigned int offset)
{
    if (!m_hasVBO)
    {
        std::cout << "[hogl:warn] No VBO specified for set_vbo_ap\n";
        return *this;
    }

    unsigned gl_type = get_type_element(etype);

    glVertexAttribPointer(slot, (GLint)count, gl_type, GL_FALSE, stride, (void*)offset);
    glEnableVertexAttribArray(slot);

    return *this;
}

hogl_obj_builder& hogl_obj_builder::set_vertices_count(unsigned int count)
{
    m_object->vertice_count = count;
    return *this;
}

hogl_obj_builder& hogl_obj_builder::set_shader(unsigned int shader)
{
    m_object->shader_id = shader;
    return *this;
}

hogl_obj_builder& hogl_obj_builder::add_shader()
{
    m_object->shader_id = glCreateProgram();
    return *this;
}

hogl_obj_builder& hogl_obj_builder::add_vertex_source(const char* source)
{
    // Create shader, set its source and the compile it
    m_tempVert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(m_tempVert, 1, &source, NULL);
    glCompileShader(m_tempVert);

    // Print information if compilation failed
    verify_shader_compilation(m_tempVert);

    return *this;
}

hogl_obj_builder& hogl_obj_builder::add_fragment_source(const char* source)
{
    // Create shader, set its source and the compile it
    m_tempFrag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(m_tempFrag, 1, &source, NULL);
    glCompileShader(m_tempFrag);

    // Print information if compilation failed
    verify_shader_compilation(m_tempFrag);

    return *this;
}

hogl_obj_builder& hogl_obj_builder::link_shader()
{
    if (m_object->shader_id == 0)
    {
        std::cout << "[hogl:warn] Shader for object is not yet created\n";
        return *this;
    }

    if (m_tempVert == 0)
    {
        std::cout << "[hogl:warn] Vertex shader either failed to be compiled or has not been created\n";
        return *this;
    }

    if (m_tempFrag == 0)
    {
        std::cout << "[hogl:warn] Fragment shader either failed to be compiled or has not been created\n";
        return *this;
    }

    // Bind, attach and link
    glAttachShader(m_object->shader_id, m_tempVert);
    glAttachShader(m_object->shader_id, m_tempFrag);
    glLinkProgram(m_object->shader_id);

    // Print link problems
    verify_shader_link(m_object->shader_id);

    // Now delete fragment and vertex shaders
    glDeleteShader(m_tempVert);
    glDeleteShader(m_tempFrag);

    m_tempVert = 0;
    m_tempFrag = 0;

    return *this;
}

HOGL_NSPACE_END
