#pragma once

#include <hogl/hogl.hpp>

#include <iostream>
#include <random>

using namespace hogl;

hogl_context* context;

hogl_ubo_layout layout;

hogl_ubo* pbr_ubo;
hogl_ubo* bg_ubo;

hogl_shader* pbrShader;
hogl_shader* equirectangularToCubemapShader;
hogl_shader* irradianceShader;
hogl_shader* prefilterShader;
hogl_shader* brdfShader;
hogl_shader* backgroundShader;

struct pbr_samplers
{
    int irradianceMap = 0;
    int prefilterMap = 1;
    int brdfLUT = 2;
    int albedoMap = 3;
    int normalMap = 4;
    int metallicMap = 5;
    int roughnessMap = 6;
    int aoMap = 7;
} ipbr_samplers;

struct bg_samplers
{
    int environmentMap = 0;
} ibg_samplers;

void mouse_key_callback(void* user_pointer, const hogl_e_mkey* event)
{
    std::cout << "Mouse key: " << event->button << "\n";
}

void key_callback(void* user_pointer, const hogl_e_key* event)
{
    std::cout << "Key: " << event->key << "\n";
}

void create_ubo_layout()
{
    // UBO layout
    layout.blocks["pbr_samplers"] = 0;
    layout.blocks["bg_samplers"] = 1;
}

void create_ubos()
{
    pbr_ubo =
        hogl_new_ubo()
        .add_ubo()
        .allocate(sizeof(pbr_samplers))
        .add_range(0, 0, sizeof(pbr_samplers))
        .ptr();

    bg_ubo =
        hogl_new_ubo()
        .add_ubo()
        .allocate(sizeof(bg_samplers))
        .add_range(0, 0, sizeof(bg_samplers))
        .ptr();
}

void load_shaders()
{
    // Load shaders
    pbrShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.pbr.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.pbr.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();

    equirectangularToCubemapShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.equirectangular_to_cubemap.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();

    irradianceShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.irradiance_convolution.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();

    prefilterShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.prefilter.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();

    brdfShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();

    backgroundShader =
        hogl_new_shader()
        .add_shader()
        .add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.vs").c_str())
        .add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.fs").c_str())
        .link_shader()
        .apply_layout(layout)
        .ptr();
}

int pbr_test()
{
    // Create context
    context = hogl_init();

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

    create_ubo_layout();
    create_ubos();
    load_shaders();

    // Map ubo data
    hogl_ubo_data(pbr_ubo, &ipbr_samplers, 0, 0);
    hogl_ubo_data(bg_ubo, &ibg_samplers, 0, 0);




    // For advanced rendering we will use hogl_render_object and hogl_render_draw_call objects
    //hogl_render_object render_object;
    //render_object.mesh = mesh;
    //render_object.shader = shader;
    //
    //hogl_render_draw_call draw_call;
    //draw_call.object = &render_object;
    //draw_call.depth_test = hogl_render_depth::LEQUAL;
    //draw_call.seamless_cubemap = true;

    // Loop until window is closed
    while (window->is_closed == 0)
    {
        // Begin a new frame
        context->renderer->begin_frame();

        // Execute draw call
        //context->renderer->execute_draw_call(&draw_call);

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
    //hogl_free_mesh(mesh);
    //hogl_free_shader(shader);
    //hogl_free_ubo(ubo);

    // Shutdown hogl
    hogl_shutdown(context);
}
