#pragma once

#include <hogl/hogl.hpp>

#include <iostream>
#include <random>

using namespace hogl;
using namespace hogl::al1;

hogl_context* context;

hogl_shader_layout sampler_layout;
hogl_shader_layout ubo_layout;

hogl_obj_handle<hogl_mesh> cubeMesh;

hogl_obj_handle<hogl_ubo> pbr_ubo;
hogl_obj_handle<hogl_ubo> bg_ubo;
hogl_obj_handle<hogl_ubo> eqt_ubo;
hogl_obj_handle<hogl_ubo> irr_ubo;

hogl_obj_handle<hogl_shader> pbrShader;
hogl_obj_handle<hogl_shader> equirectangularToCubemapShader;
hogl_obj_handle<hogl_shader> irradianceShader;
hogl_obj_handle<hogl_shader> prefilterShader;
hogl_obj_handle<hogl_shader> brdfShader;
hogl_obj_handle<hogl_shader> backgroundShader;

hogl_obj_handle<hogl_texture> ironAlbedoMap;
hogl_obj_handle<hogl_texture> ironNormalMap;
hogl_obj_handle<hogl_texture> ironMetallicMap;
hogl_obj_handle<hogl_texture> ironRoughnessMap;
hogl_obj_handle<hogl_texture> ironAOMap;

hogl_obj_handle<hogl_texture> goldAlbedoMap;
hogl_obj_handle<hogl_texture> goldNormalMap;
hogl_obj_handle<hogl_texture> goldMetallicMap;
hogl_obj_handle<hogl_texture> goldRoughnessMap;
hogl_obj_handle<hogl_texture> goldAOMap;

hogl_obj_handle<hogl_texture> grassAlbedoMap;
hogl_obj_handle<hogl_texture> grassNormalMap;
hogl_obj_handle<hogl_texture> grassMetallicMap;
hogl_obj_handle<hogl_texture> grassRoughnessMap;
hogl_obj_handle<hogl_texture> grassAOMap;

hogl_obj_handle<hogl_texture> plasticAlbedoMap;
hogl_obj_handle<hogl_texture> plasticNormalMap;
hogl_obj_handle<hogl_texture> plasticMetallicMap;
hogl_obj_handle<hogl_texture> plasticRoughnessMap;
hogl_obj_handle<hogl_texture> plasticAOMap;

hogl_obj_handle<hogl_texture> wallAlbedoMap;
hogl_obj_handle<hogl_texture> wallNormalMap;
hogl_obj_handle<hogl_texture> wallMetallicMap;
hogl_obj_handle<hogl_texture> wallRoughnessMap;
hogl_obj_handle<hogl_texture> wallAOMap;

hogl_obj_handle<hogl_texture> hdr;

hogl_obj_handle<hogl_framebuffer> fbo;

hogl_obj_handle<hogl_texture> envCubemap;
hogl_obj_handle<hogl_texture> irradianceMap;

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

struct eqt_data
{
	int equirectangularMap = 0;
	hogl_m44<float> projection;
	hogl_m44<float> view;
} eqt_data;

struct irr_data
{
	int environmentMap = 0;
	hogl_m44<float> projection;
	hogl_m44<float> view;
} irr_data;

void mouse_key_callback(void* user_pointer, const hogl_e_mkey* event)
{
	std::cout << "Mouse key: " << event->button << "\n";
}

void key_callback(void* user_pointer, const hogl_e_key* event)
{
	std::cout << "Key: " << event->key << "\n";
}

void create_layout()
{
	// UBO layout
	ubo_layout.entries["pbr_samplers"] = 0;
	ubo_layout.entries["bg_samplers"] = 1;
	ubo_layout.entries["eqt_data"] = 2;
}

void create_geometry()
{
	cubeMesh = hogl_geometry_cube();
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

	eqt_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(eqt_data))
		.add_range(0, 0, sizeof(eqt_data))
		.ptr();

	irr_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(irr_data))
		.add_range(0, 0, sizeof(irr_data))
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
		.ubo_layout(ubo_layout)
		.ptr();

	equirectangularToCubemapShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.equirectangular_to_cubemap.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.ptr();

	irradianceShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.irradiance_convolution.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.ptr();

	prefilterShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.prefilter.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.ptr();

	brdfShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.ptr();

	backgroundShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.ptr();
}

hogl_texture* load_2d_texture(const char* path)
{
	hogl_loader_image<unsigned char> image_data;
	context->loader->load_image(&image_data, path);

	// Create texture
	return 
		hogl_new_texture()
		.add_texture()
		.add_image(&image_data)
		.generate_mipmap()
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::REPEAT)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::REPEAT)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.set_min_filter(hogl_filter_type::LINEAR_MIPMAP_LINEAR)
		.ptr();
}

hogl_texture* load_hdr_texture(const char* path)
{
	hogl_loader_image<float> image_data;
	context->loader->load_hdr(&image_data, path);

	// Create texture
	return
		hogl_new_texture()
		.add_texture()
		.add_hdr(&image_data)
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::EDGE_CLAMP)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.set_min_filter(hogl_filter_type::LINEAR)
		.ptr();
}

void load_textures()
{
	ironAlbedoMap.own(load_2d_texture("res/pbr/rusted_iron/albedo.png"));
	ironNormalMap.own(load_2d_texture("res/pbr/rusted_iron/normal.png"));
	ironMetallicMap.own(load_2d_texture("res/pbr/rusted_iron/metallic.png"));
	ironRoughnessMap.own(load_2d_texture("res/pbr/rusted_iron/roughness.png"));
	ironAOMap.own(load_2d_texture("res/pbr/rusted_iron/ao.png"));

	goldAlbedoMap.own(load_2d_texture("res/pbr/gold/albedo.png"));
	goldNormalMap.own(load_2d_texture("res/pbr/gold/normal.png"));
	goldMetallicMap.own(load_2d_texture("res/pbr/gold/metallic.png"));
	goldRoughnessMap.own(load_2d_texture("res/pbr/gold/roughness.png"));
	goldAOMap.own(load_2d_texture("res/pbr/gold/ao.png"));

	grassAlbedoMap.own(load_2d_texture("res/pbr/grass/albedo.png"));
	grassNormalMap.own(load_2d_texture("res/pbr/grass/normal.png"));
	grassMetallicMap.own(load_2d_texture("res/pbr/grass/metallic.png"));
	grassRoughnessMap.own(load_2d_texture("res/pbr/grass/roughness.png"));
	grassAOMap.own(load_2d_texture("res/pbr/grass/ao.png"));

	plasticAlbedoMap.own(load_2d_texture("res/pbr/plastic/albedo.png"));
	plasticNormalMap.own(load_2d_texture("res/pbr/plastic/normal.png"));
	plasticMetallicMap.own(load_2d_texture("res/pbr/plastic/metallic.png"));
	plasticRoughnessMap.own(load_2d_texture("res/pbr/plastic/roughness.png"));
	plasticAOMap.own(load_2d_texture("res/pbr/plastic/ao.png"));

	wallAlbedoMap.own(load_2d_texture("res/pbr/wall/albedo.png"));
	wallNormalMap.own(load_2d_texture("res/pbr/wall/normal.png"));
	wallMetallicMap.own(load_2d_texture("res/pbr/wall/metallic.png"));
	wallRoughnessMap.own(load_2d_texture("res/pbr/wall/roughness.png"));
	wallAOMap.own(load_2d_texture("res/pbr/wall/ao.png"));
}

void setup_fbo()
{
	// Create texture
	fbo.own(
		hogl_new_framebuffer()
			.add_fbo()
			.attach_renderbuffer(512, 512, hogl_rbuffer_format::d24)
			.ptr()
	);
}

int test()
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

	create_geometry();
	create_layout();
	create_ubos();
	load_shaders();

	// Map ubo data
	hogl_ubo_data(pbr_ubo, &ipbr_samplers, 0, 0);
	hogl_ubo_data(bg_ubo, &ibg_samplers, 0, 0);

	// Load textures
	load_textures();

	// Lights
	float lightPositions[] = {
		-10.0f,  10.0f, 10.0f,
		 10.0f,  10.0f, 10.0f,
		-10.0f, -10.0f, 10.0f,
		 10.0f, -10.0f, 10.0f,
	};

	float lightColors[] = {
		300.0f, 300.0f, 300.0f,
		300.0f, 300.0f, 300.0f,
		300.0f, 300.0f, 300.0f,
		300.0f, 300.0f, 300.0f
	};

	// Setup framebuffer
	setup_fbo();

	// Load HDR image
	hdr.own(load_hdr_texture("res/hdr/newport_loft.hdr"));

	// Create environment cube map
	// We set the auto increment and then allocate 6 512x512 textures then set up wrapping and filters 
	envCubemap = hogl_new_texture()
		.add_texture()
		.cslot_auto_increment(true)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.alloc(512, 512, hogl_texture_format::RGB16F)
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Z, hogl_wrap_type::EDGE_CLAMP)
		.set_min_filter(hogl_filter_type::LINEAR_MIPMAP_LINEAR)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.ptr();

	// Setup cubemap view matrices, could be done in pure math functions but we will take advantage of the 1 abstraction layer
	hogl_m44<float> captureProjection = hogl_perspective(90.0f, 1.0f, 0.1f, 10.0f);

	hogl_m44<float> captureViews[] =
	{
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(1.0f,  0.0f,  0.0f),  hogl_v3<float>(0.0f, -1.0f,  0.0f)),
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(-1.0f,  0.0f,  0.0f), hogl_v3<float>(0.0f, -1.0f,  0.0f)),
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(0.0f,  1.0f,  0.0f),  hogl_v3<float>(0.0f,  0.0f,  1.0f)),
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(0.0f, -1.0f,  0.0f),  hogl_v3<float>(0.0f,  0.0f, -1.0f)),
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(0.0f,  0.0f,  1.0f),  hogl_v3<float>(0.0f, -1.0f,  0.0f)),
		hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 0.0f), hogl_v3<float>(0.0f,  0.0f, -1.0f),  hogl_v3<float>(0.0f, -1.0f,  0.0f))
	};

	// Map eqt data
	eqt_data.projection = captureProjection;
	irr_data.projection = captureProjection;

	// Draw call to the framebuffer

	// Create render target for our environment map 
	hogl_fbo_render_target* cubemap_target = new hogl_fbo_render_target(fbo);

	// Set clear color
	target->set_clear_color(0, 0, 0, 0);
	target->attach_texture(hdr);

	// Attach to the renderer
	context->renderer->change_target((hogl_i_render_target*)target);

	// Now we need to render from the 6 different view locations

	// Adjust viewport first
	context->renderer->adjust_viewport(512, 512);

	// Create cube render object
	hogl_render_object cubeRender;
	cubeRender.mesh = cubeMesh;
	cubeRender.shader = equirectangularToCubemapShader;
	cubeRender.textures = { hdr };

	hogl_render_draw_call draw_call;
	draw_call.object = &cubeRender;
	draw_call.depth_test = hogl_render_depth::LEQUAL;
	draw_call.seamless_cubemap = true;

	// We render into our empty cube map, since we allocated it with nullptr at the beginning
	for (unsigned int i = 0; i < 6; i++)
	{
		// Assign the new viewpoint
		eqt_data.view = captureViews[i];
		hogl_ubo_data(eqt_ubo, &eqt_data, 0, 0);

		// Since we are rendering into a cube map we will need to change the internal fob target to each cube map side
		cubemap_target->set_cslot(i);

		// Begin frame
		context->renderer->begin_frame();

		// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
		context->renderer->execute_draw_call(&draw_call);

		// End the frame for the fbo
		context->renderer->end_frame();
		context->renderer->flush();
	}

	// Generate environment map mipmaps
	// We first get the pointer from the handle than call generate mipmap call
	{
		hogl_bldr_texture bldr(envCubemap.own());

		// Since we don't keep cube map information we flag it as cube map and the generate the mipmaps
		// Then return the pointer to the resource and own it with the handle
		envCubemap.own(
			bldr.set_cubemap()
			.generate_mipmap()
			.ptr()
		);
	}

	// Create irriadiance cube map
	irradianceMap = hogl_new_texture()
		.add_texture()
		.cslot_auto_increment(true)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.alloc(32, 32, hogl_texture_format::RGB16F)
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Z, hogl_wrap_type::EDGE_CLAMP)
		.set_min_filter(hogl_filter_type::LINEAR)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.ptr();

	// Adjust viewport
	context->renderer->adjust_viewport(32, 32);

	// Change internal render buffer dimensions
	hogl_adjust_rbo(fbo, hogl_rbuffer_format::d24, 32, 32);

	// Change attached texture
	target->attach_texture(irradianceMap);

	// Create cube render object
	cubeRender.shader = irradianceShader;
	cubeRender.textures = { irradianceMap };

	// Reset the object since we just changed the textures and haven't executed any other draw call in between
	context->renderer->reset_object();

	// We render into our empty cube map, since we allocated it with nullptr at the beginning
	for (unsigned int i = 0; i < 6; i++)
	{
		// Assign the new viewpoint
		eqt_data.view = captureViews[i];
		hogl_ubo_data(eqt_ubo, &eqt_data, 0, 0);

		// Since we are rendering into a cube map we will need to change the internal fob target to each cube map side
		cubemap_target->set_cslot(i);

		// Begin frame
		context->renderer->begin_frame();

		// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
		context->renderer->execute_draw_call(&draw_call);

		// End the frame for the fbo
		context->renderer->end_frame();
		context->renderer->flush();
	}

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

	// Shutdown hogl
	hogl_shutdown(context);
}

/*

// Create input interface
hogl_e_interface* input_interface = context->input_system->new_interface();

// Set callbacks
input_interface->cb_key = key_callback;
input_interface->cb_mkey = mouse_key_callback;

*/