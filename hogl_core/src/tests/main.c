#include <stdio.h>
#include <stdlib.h>

#include "hogl_core/hogl_core.h"

#include <gl/glad.h>
#include <gl/glfw3.h>

hogl_vao* example_vao;
hogl_shader* example_shader;

void create_primitives(void) {
	hogl_vbo_desc* vaodescs[2]; // 2 VBOS 2 descriptions

	hogl_vbo_desc vdesc;
	hogl_vbo_desc idesc;

	hogl_shader_desc sdesc;

	// VAO
	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	unsigned int indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	// Fill array
	vaodescs[0] = &vdesc;
	vaodescs[1] = &idesc;


	// Set type
	vdesc.type = HOGL_VBOT_ARRAY_BUFFER;
	idesc.type = HOGL_VBOT_ELEMENT_BUFFER;

	// Usage
	vdesc.usage = HOGL_VBOU_STATIC;
	idesc.usage = HOGL_VBOU_STATIC;

	// Attribute pointer description, element buffers don't need this
	vdesc.ap_desc.index = 0;					// 0 index
	vdesc.ap_desc.ecount = 3;					// 3 elements per entry
	vdesc.ap_desc.type = HOGL_ET_FLOAT;			// Data is in floats
	vdesc.ap_desc.normalized = false;			// Don't normalize
	vdesc.ap_desc.stride = 3 * sizeof(float);	// Single vertex entry stride
	vdesc.ap_desc.offset = 0;					// Start from 0
	vdesc.ap_desc.divisor = 0;					// No instanced rendering

	// Size and data
	vdesc.size = sizeof(vertices);	// Or 12 * sizeof(float)
	idesc.size = sizeof(indices);	// Or 6 * sizoef(unsigned int)

	vdesc.data = vertices;
	idesc.data = indices;

	// Finally create a vao
	hogl_vao_new(&example_vao);

	// After allocating buffers the internal vbos are stored as:
	// 0 - vertices array
	// 1 - indices array
	// Same as the specified descs array
	if (hogl_vao_alloc_buffers(example_vao, vaodescs, 2) != HOGL_ERROR_NONE) {
		exit(-1);
	}

	// Shader
	sdesc.vertex_source = 
		"#version 330 core\n\
		layout(location = 0) in vec3 aPos;\n\
		void main()\n\
		{\n\
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
		}";

	sdesc.fragment_source = 
		"#version 330 core\n\
		out vec4 FragColor;\n\
		void main()\n\
		{\n\
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n\
		}";

	if (hogl_shader_new(&example_shader, sdesc) != HOGL_ERROR_NONE) {
		exit(-1);
	}
}

int main(int argc, char** argv) {
	hogl_wnd* hwindow = NULL;
	hogl_wi* hwi = NULL;
	hogl_vf* resources = NULL;

	// Initialize hogl
	if (hogl_init() != HOGL_ERROR_NONE) {
		// Failed to initialize hogl
		return 1;
	}

	// Create new window
	if (hogl_new_window(&hwindow) != HOGL_ERROR_NONE) {
		// Failed to create a window
		return 3;
	}

	// Not necessary but just configures to the default state
	hogl_set_rstate(hogl_default_rstate());

	// Get window interface
	hwi = hogl_get_wi_ptr(hwindow);

	// We create OpenGL primitives
	create_primitives();

	hogl_shader_bind(example_shader);
	hogl_vao_bind(example_vao);

	while (hwi->is_open) {
		hogl_render_clear(0.0f, 1.0f, 0.0f, 1.0f);
		hogl_render_e(HOGL_RM_TRIANGLES, 6);

		// Swap buffer and check for events
		hogl_swap_window_buffer(hwindow);
		hogl_update_window_states();
	}

	hogl_vao_free(example_vao);
	hogl_shader_free(example_shader);

	hogl_destroy_window(hwindow);
	hogl_vf_free(resources);

	hogl_shutdown();

	return 0;
}
