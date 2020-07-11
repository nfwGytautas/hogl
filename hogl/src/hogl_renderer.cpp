#include "hogl_renderer.hpp"

#include <iostream>

#include <gl/glad.h>
#include <gl/glfw3.h>

#include "hogl_wnd.hpp"
#include "hogl_object.hpp"

HOGL_NSPACE_BEGIN

void hogl_cs_renderer::change_target(hogl_i_render_target* target)
{
	m_target = target;

	if (m_target != nullptr && m_target->valid())
	{
		m_target->bind();
	}
	else
	{
		HOGL_LOG_ERROR("Bad render target!");
		m_target = nullptr;
	}
}

void hogl_cs_renderer::begin_frame()
{
	if (m_target != nullptr && m_target->valid())
	{
		// Clear the target
		m_target->clear();
	}
}

void hogl_cs_renderer::render_object(hogl_render_object* object)
{
	if (m_target != nullptr && m_target->valid())
	{
		// Bind object and render
		bind_object(object);
		render_single();
	}
}

void hogl_cs_renderer::execute_draw_call(hogl_render_draw_call* draw_call)
{
	if (m_target != nullptr && m_target->valid())
	{
		// Bind draw call and render
		bind_draw_call(draw_call);
		render_single();
	}
}

void hogl_cs_renderer::end_frame()
{
	if (m_target != nullptr && m_target->valid())
	{

	}
}

void hogl_cs_renderer::flush()
{
	if (m_target != nullptr && m_target->valid())
	{
		// Flush the OpenGL contents
		m_target->flush();
	}
}

void hogl_cs_renderer::bind_draw_call(hogl_render_draw_call* draw_call)
{
	if (draw_call == nullptr)
	{
		HOGL_LOG_ERROR("Trying to bind a nullptr draw call!");
		return;
	}

	// Check if there is an actual need to bind
	if (m_currentDrawCall == draw_call)
	{
		return;
	}

	// Check what state to change
	if (m_currentDrawCall != nullptr)
	{
		if (m_currentDrawCall->depth_test != draw_call->depth_test)
		{
			set_depth_func(draw_call->depth_test);
		}
		if (m_currentDrawCall->seamless_cubemap != draw_call->seamless_cubemap)
		{
			set_seamless_cubemap(draw_call->seamless_cubemap);
		}
	}
	else
	{
		// Defaults
		set_depth_func(hogl_render_depth::LESS);
		set_seamless_cubemap(true);
	}

	m_currentDrawCall = draw_call;
	bind_object(m_currentDrawCall->object);
}

void hogl_cs_renderer::bind_object(hogl_render_object* object)
{
	if (object == nullptr)
	{
		HOGL_LOG_ERROR("Trying to bind a nullptr object!");
		return;
	}

	// Check if there is an actual need to bind
	if (m_currentObject == object)
	{
		return;
	}

	m_currentObject = object;
	bind_mesh(object->mesh);
	bind_shader(object->shader);

	for (hogl_texture* texture : object->textures)
	{
		bind_texture(texture);
	}
}

void hogl_cs_renderer::bind_mesh(hogl_mesh* mesh)
{
	if (mesh == nullptr)
	{
		HOGL_LOG_ERROR("Trying to bind a nullptr mesh!");
		return;
	}

	// Check if there is an actual need to bind
	if (m_currentMesh == mesh)
	{
		return;
	}

	m_currentMesh = mesh;
	glBindVertexArray(m_currentMesh->vao_id);

	if (m_currentMesh->ebo_id != 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_currentMesh->ebo_id);
	}
}

void hogl_cs_renderer::bind_shader(hogl_shader* shader)
{
	if (shader == nullptr)
	{
		HOGL_LOG_ERROR("Trying to bind a nullptr shader!");
		return;
	}

	// Check if there is an actual need to bind
	if (m_currentShader == shader)
	{
		return;
	}

	m_currentShader = shader;
	glUseProgram(m_currentShader->shader_id);
}

void hogl_cs_renderer::bind_texture(hogl_texture* texture)
{
	if (texture == nullptr)
	{
		HOGL_LOG_ERROR("Trying to bind a nullptr texture!");
		return;
	}

	// Check if there is an actual need to bind
	if (m_currentTexture[texture->slot] == texture)
	{
		return;
	}

	m_currentTexture[texture->slot] = texture;
	glActiveTexture(GL_TEXTURE0 + texture->slot);
	glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

void hogl_cs_renderer::render_single()
{
	if (m_currentMesh == nullptr)
	{
		HOGL_LOG_ERROR("Trying to render a nullptr mesh!");
		return;
	}

	if (m_currentMesh->ebo_id == 0)
	{
		glDrawArrays(GL_TRIANGLES, 0, m_currentMesh->vertice_count);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, m_currentMesh->vertice_count, GL_UNSIGNED_INT, 0);
	}
}

void hogl_cs_renderer::set_depth_func(hogl_render_depth func)
{
	if (func == hogl_render_depth::DISABLED)
	{
		glDisable(GL_DEPTH_TEST);
		return;
	}

	// Set depth function
	glEnable(GL_DEPTH_TEST);
	switch (func)
	{		
	case hogl_render_depth::ALWAYS:
		glDepthFunc(GL_ALWAYS);
		break;
	case hogl_render_depth::NEVER:
		glDepthFunc(GL_NEVER);
		break;
	case hogl_render_depth::LESS:
		glDepthFunc(GL_LESS);
		break;
	case hogl_render_depth::EQUAL:
		glDepthFunc(GL_EQUAL);
		break;
	case hogl_render_depth::LEQUAL:
		glDepthFunc(GL_LEQUAL);
		break;
	case hogl_render_depth::GREATER:
		glDepthFunc(GL_GREATER);
		break;
	case hogl_render_depth::NOTEQUAL:
		glDepthFunc(GL_NOTEQUAL);
		break;
	case hogl_render_depth::GEQUAL:
		glDepthFunc(GL_GEQUAL);
		break;
	}
}

void hogl_cs_renderer::set_seamless_cubemap(bool value)
{
	if (value)
	{
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
	else
	{
		glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}
}

hogl_wnd_render_target::hogl_wnd_render_target(hogl_wnd* wnd)
	: m_wnd(wnd), m_clearColor()
{
}

void hogl_wnd_render_target::set_clear_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
	if (r <= 255)
	{
		m_clearColor[0] = (float)r / 255;
	}

	if (g <= 255)
	{
		m_clearColor[1] = (float)g / 255;
	}

	if (b <= 255)
	{
		m_clearColor[2] = (float)b / 255;
	}

	if (a <= 255)
	{
		m_clearColor[3] = (float)a / 255;
	}
}

bool hogl_wnd_render_target::valid()
{
	return m_wnd != nullptr && m_wnd->native_window != nullptr && m_wnd->destroy == 0;
}

void hogl_wnd_render_target::bind()
{
	// Make the window as the current OpenGL context
	glfwMakeContextCurrent((GLFWwindow*)m_wnd->native_window);
}

void hogl_wnd_render_target::clear()
{
	glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2], m_clearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void hogl_wnd_render_target::flush()
{
	glfwSwapBuffers((GLFWwindow*)m_wnd->native_window);
}

HOGL_NSPACE_END
