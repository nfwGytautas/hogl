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
		std::cout << "[hogl:error] Bad render target\n";
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

void hogl_cs_renderer::render_object(hogl_object* object)
{
	if (m_target != nullptr && m_target->valid())
	{
		glUseProgram(object->shader_id);
		glBindVertexArray(object->vao_id);

		if (object->ebo_id == 0)
		{
			glDrawArrays(GL_TRIANGLES, 0, object->vertice_count);
		}
		else
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->ebo_id);
			glDrawElements(GL_TRIANGLES, object->vertice_count, GL_UNSIGNED_INT, 0);
		}
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
