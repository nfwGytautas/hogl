#pragma once

#include <hogl/hogl.hpp>

#include <iostream>
#include <random>

using namespace hogl;

void mouse_key_callback(void* user_pointer, const hogl_e_mkey* event)
{
    std::cout << "Mouse key: " << event->button << "\n";
}

void key_callback(void* user_pointer, const hogl_e_key* event)
{
    std::cout << "Key: " << event->key << "\n";
}

int texture_test()
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

    // Sampler layout
    hogl_shader_layout sampler_layout;
    sampler_layout.entries["texture1"] = 0;
    sampler_layout.entries["texture2"] = 1;

    // Create a mesh and a shader to render it with
    float vertices[] = {
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    hogl_mesh* mesh =
        hogl_new_mesh()
        .add_vbo(hogl_vbo_type::ARRAY_BUFFER)
        .add_vbo_data(hogl_vbo_usage::STATIC, sizeof(vertices), vertices)
        .set_vbo_ap(0, hogl_ap_type::FLOAT, hogl_ap_count::THREE, 5 * sizeof(float), 0)
        .set_vbo_ap(1, hogl_ap_type::FLOAT, hogl_ap_count::TWO, 5 * sizeof(float), 3 * sizeof(float))
        .add_vbo(hogl_vbo_type::ELEMENT_BUFFER)
        .add_vbo_data(hogl_vbo_usage::STATIC, sizeof(indices), indices)
        .ptr();

    hogl_shader* shader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/test/0.0.1.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/test/0.0.1.fs").c_str())
        .link_shader()
        .sampler_layout(sampler_layout)
        .ptr();

    // Load image data
    hogl_loader_image image1_data;
    context->loader->load_image(&image1_data, "res/pbr/rusted_iron/albedo.png");

    hogl_loader_image image2_data;
    context->loader->load_image(&image2_data, "res/pbr/grass/albedo.png");

    // Create texture
    hogl_texture* texture1 =
        hogl_new_texture()
        .add_texture()
        .add_data(&image1_data)
        .generate_mipmap()
        .set_wrap(hogl_wrap_axis::X, hogl_wrap_type::REPEAT)
        .set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::REPEAT)
        .set_mag_filter(hogl_filter_type::NEAREST)
        .set_min_filter(hogl_filter_type::LINEAR_MIPMAP_LINEAR)
        .slot(0)
        .ptr();

    hogl_texture* texture2 =
        hogl_new_texture()
        .add_texture()
        .add_data(&image2_data)
        .generate_mipmap()
        .set_wrap(hogl_wrap_axis::X, hogl_wrap_type::REPEAT)
        .set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::REPEAT)
        .set_mag_filter(hogl_filter_type::NEAREST)
        .set_min_filter(hogl_filter_type::LINEAR_MIPMAP_LINEAR)
        .slot(1)
        .ptr();

    // For advanced rendering we will use hogl_render_object and hogl_render_draw_call objects
    hogl_render_object render_object;
    render_object.mesh = mesh;
    render_object.shader = shader;
    render_object.textures = { texture1, texture2 };

    hogl_render_draw_call draw_call;
    draw_call.object = &render_object;
    draw_call.depth_test = hogl_render_depth::LEQUAL;
    draw_call.seamless_cubemap = true;

    // Loop until window is closed
    while (window->is_closed == 0)
    {
        // Begin a new frame
        context->renderer->begin_frame();

        // Execute draw call
        context->renderer->execute_draw_call(&draw_call);

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
    hogl_free_mesh(mesh);
    hogl_free_shader(shader);
    hogl_free_texture(texture1);
    hogl_free_texture(texture2);

    // Shutdown hogl
    hogl_shutdown(context);
}
