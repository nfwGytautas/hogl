#include <hogl/hogl.hpp>

#include <iostream>

using namespace hogl;

void mouse_key_callback(void* user_pointer, const hogl_e_mkey* event)
{
    std::cout << "Mouse key: " << event->button << "\n";
}

void key_callback(void* user_pointer, const hogl_e_key* event)
{
    std::cout << "Key: " << event->key << "\n";
}

int main()
{
    // Create context
    hogl_context* context = hogl_init();

    if (context == nullptr)
    {
        return 1;
    }

    // Create window
    hogl_wnd* window = context->window_manager->create();

    // Bind window to the input system
    context->input_system->bind_wnd(window);

    // Create input interface
    hogl_e_interface* input_interface = context->input_system->new_interface();

    // Set callbacks
    input_interface->cb_key = key_callback;
    input_interface->cb_mkey = mouse_key_callback;

    // Create render target for the window
    hogl_wnd_render_target* target = new hogl_wnd_render_target(window);

    // Set clear color
    target->set_clear_color(125, 200, 100, 255);

    // Attach to the renderer
    context->renderer->change_target((hogl_i_render_target*)target);

    // Create a hogl object
    hogl_object* object = new hogl_object();
    hogl_obj_builder builder(object);

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    builder.init()
        .add_vbo(hogl_vbo_type::ARRAY_BUFFER)
        .add_vbo_data(hogl_vbo_usage::STATIC, sizeof(vertices), vertices)
        .set_vbo_ap(0, hogl_ap_type::FLOAT, hogl_ap_count::THREE, 3 * sizeof(float), 0)
        .add_vbo(hogl_vbo_type::ELEMENT_BUFFER)
        .add_vbo_data(hogl_vbo_usage::STATIC, sizeof(indices), indices)
        .add_shader()
        .add_vertex_source(
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0"
        )
        .add_fragment_source(
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
            "}\n\0"
        )
        .link_shader();

    // Loop until window is closed
    while (window->is_closed == 0)
    {
        // Begin a new frame
        context->renderer->begin_frame();

        // Render the object
        context->renderer->render_object(object);

        // End frame and flush
        context->renderer->end_frame();
        context->renderer->flush();

        // Updates window state this includes is_closed flag
        context->window_manager->update();

        // Polls hogl input
        context->input_system->update();
    }

    // Free resources
    delete target;
    delete object;

    // Shutdown hogl
    hogl_shutdown(context);
}
