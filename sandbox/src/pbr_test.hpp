#pragma once

#include <hogl/hogl.hpp>

#include <iostream>
#include <random>
#include <cmath>

using namespace hogl;
using namespace hogl::al1;

hogl_context* context;

hogl_shader_layout sampler_layout;
hogl_shader_layout ubo_layout;

hogl_obj_handle<hogl_mesh> cubeMesh;
hogl_obj_handle<hogl_mesh> quadMesh;
hogl_obj_handle<hogl_mesh> sphereMesh;

hogl_obj_handle<hogl_ubo> pbr_ubo;
hogl_obj_handle<hogl_ubo> bg_ubo;
hogl_obj_handle<hogl_ubo> eqt_ubo;
hogl_obj_handle<hogl_ubo> irr_ubo;
hogl_obj_handle<hogl_ubo> prefilter_ubo;
hogl_obj_handle<hogl_ubo> pbr_data_ubo;
hogl_obj_handle<hogl_ubo> bg_data_ubo;
hogl_obj_handle<hogl_ubo> matrices_ubo;

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
hogl_obj_handle<hogl_texture> prefilterMap;
hogl_obj_handle<hogl_texture> brdfLUTTexture;

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
} eqt_data;

struct irr_data
{
	int environmentMap = 0;
} irr_data;

struct prefilter_data
{
	float roughness = 0.0f;
} prefilter_data;

struct pbr_data
{
	hogl_m44<float> model;

	// UBO requires alignment multiple of 16 so vec3 can't be used as this would cause data loss
	hogl_v4<float> lightPosition[4];
	hogl_v4<float> lightColor[4];

	hogl_v4<float> camPos;
} pbr_data;

struct matrices_data
{
	hogl_m44<float> projection;
	hogl_m44<float> view;
} matrices_data;

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
	ubo_layout.entries["irr_data"] = 3;
	ubo_layout.entries["prefilter_data"] = 4;
	ubo_layout.entries["pbr_data"] = 5;
	ubo_layout.entries["bg_data"] = 6;
	ubo_layout.entries["matrices"] = 7;

	// Sampler layout
	sampler_layout.entries["environmentMap"] = 0;
	sampler_layout.entries["equirectangularMap"] = 0;
	sampler_layout.entries["irradianceMap"] = 0;
	sampler_layout.entries["prefilterMap"] = 1;
	sampler_layout.entries["brdfLUT"] = 2;
	sampler_layout.entries["albedoMap"] = 3;
	sampler_layout.entries["normalMap"] = 4;
	sampler_layout.entries["metallicMap"] = 5;
	sampler_layout.entries["roughnessMap"] = 6;
	sampler_layout.entries["aoMap"] = 7;
}

void create_geometry()
{
	// Note the own calls here. Because of implicit copy operator when assigning handles returned from functions
	// these handles will trigger destruction. To avoid this by calling own on the returned handle. This is only needed
	// when assigning to global or class member.
	cubeMesh = hogl_geometry_cube().own();
	quadMesh = hogl_geometry_quad().own();
	sphereMesh = hogl_geometry_sphere().own();
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
		.add_range(1, 0, sizeof(bg_samplers))
		.ptr();

	eqt_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(eqt_data))
		.add_range(2, 0, sizeof(eqt_data))
		.ptr();

	irr_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(irr_data))
		.add_range(3, 0, sizeof(irr_data))
		.ptr();

	prefilter_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(prefilter_data))
		.add_range(4, 0, sizeof(prefilter_data))
		.ptr();

	pbr_data_ubo =
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(pbr_data))
		.add_range(5, 0, sizeof(pbr_data))
		.ptr();

	//bg_data_ubo =
	//	hogl_new_ubo()
	//	.add_ubo()
	//	.allocate(sizeof(bg_data))
	//	.add_range(6, 0, sizeof(bg_data))
	//	.ptr();

	matrices_ubo = 
		hogl_new_ubo()
		.add_ubo()
		.allocate(sizeof(matrices_data))
		.add_range(7, 0, sizeof(matrices_data))
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
		.sampler_layout(sampler_layout)
		.ptr();

	equirectangularToCubemapShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.equirectangular_to_cubemap.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.sampler_layout(sampler_layout)
		.ptr();

	irradianceShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.irradiance_convolution.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.sampler_layout(sampler_layout)
		.ptr();

	prefilterShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.cubemap.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.prefilter.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.sampler_layout(sampler_layout)
		.ptr();

	brdfShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.brdf.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.sampler_layout(sampler_layout)
		.ptr();

	backgroundShader =
		hogl_new_shader()
		.add_shader()
		.add_vertex_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.vs").c_str())
		.add_fragment_source(context->loader->read_text_file("shaders/pbr/2.2.2.background.fs").c_str())
		.link_shader()
		.ubo_layout(ubo_layout)
		.sampler_layout(sampler_layout)
		.ptr();
}

hogl_texture* load_2d_texture(const char* path, unsigned int slot = 0)
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
		.slot(slot)
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
		.slot(0)
		.ptr();
}

void load_textures()
{
	ironAlbedoMap.own(load_2d_texture("res/pbr/rusted_iron/albedo.png", 3));
	ironNormalMap.own(load_2d_texture("res/pbr/rusted_iron/normal.png", 4));
	ironMetallicMap.own(load_2d_texture("res/pbr/rusted_iron/metallic.png", 5));
	ironRoughnessMap.own(load_2d_texture("res/pbr/rusted_iron/roughness.png", 6));
	ironAOMap.own(load_2d_texture("res/pbr/rusted_iron/ao.png", 7));

	goldAlbedoMap.own(load_2d_texture("res/pbr/gold/albedo.png", 3));
	goldNormalMap.own(load_2d_texture("res/pbr/gold/normal.png", 4));
	goldMetallicMap.own(load_2d_texture("res/pbr/gold/metallic.png", 5));
	goldRoughnessMap.own(load_2d_texture("res/pbr/gold/roughness.png", 6));
	goldAOMap.own(load_2d_texture("res/pbr/gold/ao.png", 7));

	grassAlbedoMap.own(load_2d_texture("res/pbr/grass/albedo.png", 3));
	grassNormalMap.own(load_2d_texture("res/pbr/grass/normal.png", 4));
	grassMetallicMap.own(load_2d_texture("res/pbr/grass/metallic.png", 5));
	grassRoughnessMap.own(load_2d_texture("res/pbr/grass/roughness.png", 6));
	grassAOMap.own(load_2d_texture("res/pbr/grass/ao.png", 7));

	plasticAlbedoMap.own(load_2d_texture("res/pbr/plastic/albedo.png", 3));
	plasticNormalMap.own(load_2d_texture("res/pbr/plastic/normal.png", 4));
	plasticMetallicMap.own(load_2d_texture("res/pbr/plastic/metallic.png", 5));
	plasticRoughnessMap.own(load_2d_texture("res/pbr/plastic/roughness.png", 6));
	plasticAOMap.own(load_2d_texture("res/pbr/plastic/ao.png", 7));

	wallAlbedoMap.own(load_2d_texture("res/pbr/wall/albedo.png", 3));
	wallNormalMap.own(load_2d_texture("res/pbr/wall/normal.png", 4));
	wallMetallicMap.own(load_2d_texture("res/pbr/wall/metallic.png", 5));
	wallRoughnessMap.own(load_2d_texture("res/pbr/wall/roughness.png", 6));
	wallAOMap.own(load_2d_texture("res/pbr/wall/ao.png", 7));
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
	context->log_handle->gl_log = hogl_gl_log_level::ERR;

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
	hogl_v3<float> lightPositions[] = {
		hogl_v3<float>(-10.0f,  10.0f, 10.0f),
		hogl_v3<float>(10.0f,  10.0f, 10.0f),
		hogl_v3<float>(-10.0f, -10.0f, 10.0f),
		hogl_v3<float>(10.0f, -10.0f, 10.0f),
	};

	hogl_v3<float> lightColors[] = {
		hogl_v3<float>(300.0f, 300.0f, 300.0f),
		hogl_v3<float>(300.0f, 300.0f, 300.0f),
		hogl_v3<float>(300.0f, 300.0f, 300.0f),
		hogl_v3<float>(300.0f, 300.0f, 300.0f)
	};

	// Setup framebuffer
	setup_fbo();

	// Load HDR image
	hdr.own(load_hdr_texture("res/hdr/newport_loft.hdr"));

	// Create environment cube map
	// We set the auto increment and then allocate 6 512x512 textures then set up wrapping and filters 
	envCubemap = hogl_new_texture()
		.add_texture()
		.set_cubemap()
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
		.slot(0)
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
	matrices_data.projection = captureProjection;

	// Draw call to the framebuffer

	// Create render target for our environment map 
	hogl_fbo_render_target* cubemap_target = new hogl_fbo_render_target(fbo);

	// Set clear color
	cubemap_target->set_clear_color(0, 0, 0, 0);

	// Attach to the renderer
	context->renderer->change_target((hogl_i_render_target*)cubemap_target);

	// Now we need to render from the 6 different view locations
	cubemap_target->attach_texture(envCubemap);

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
		matrices_data.view = captureViews[i];
		hogl_ubo_data(matrices_ubo, &matrices_data, 0, 0);

		cubemap_target->bind();

		// Since we are rendering into a cube map we will need to change the internal fob target to each cube map side
		cubemap_target->set_cslot(i);

		// Begin frame
		context->renderer->begin_frame();		

		// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
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
		.set_cubemap()
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
		.slot(0)
		.ptr();

	// Change internal render buffer dimensions
	hogl_adjust_rbo(fbo, hogl_rbuffer_format::d24, 32, 32);

	// Adjust viewport
	context->renderer->adjust_viewport(32, 32);

	// Change attached texture
	cubemap_target->attach_texture(irradianceMap);

	// Create cube render object
	cubeRender.shader = irradianceShader;
	cubeRender.textures = { envCubemap };

	// Reset the object since we just changed the textures and haven't executed any other draw call in between
	context->renderer->reset_object();

	// We render into our empty cube map, since we allocated it with nullptr at the beginning
	for (unsigned int i = 0; i < 6; i++)
	{
		// Assign the new viewpoint
		matrices_data.view = captureViews[i];
		hogl_ubo_data(matrices_ubo, &matrices_data, 0, 0);

		cubemap_target->bind();

		// Since we are rendering into a cube map we will need to change the internal fob target to each cube map side
		cubemap_target->set_cslot(i);

		// Begin frame
		context->renderer->begin_frame();

		// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&draw_call);

		// End the frame for the fbo
		context->renderer->end_frame();
		context->renderer->flush();
	}

	// Create irriadiance cube map
	prefilterMap = hogl_new_texture()
		.add_texture()
		.set_cubemap()
		.cslot_auto_increment(true)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.alloc(128, 128, hogl_texture_format::RGB16F)
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Z, hogl_wrap_type::EDGE_CLAMP)
		.set_min_filter(hogl_filter_type::LINEAR_MIPMAP_LINEAR)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.generate_mipmap()
		.slot(1)
		.ptr();

	// Create cube render object
	cubeRender.shader = prefilterShader;
	cubeRender.textures = { envCubemap };

	// Attach prefilter map
	cubemap_target->attach_texture(prefilterMap);

	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// Resize framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);

		// Change internal render buffer dimensions
		hogl_adjust_rbo(fbo, hogl_rbuffer_format::d24, mipWidth, mipHeight);

		context->renderer->adjust_viewport(mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilter_data.roughness = roughness;
		hogl_ubo_data(prefilter_ubo, &prefilter_data, 0, 0);

		// Reset the object since we just changed the textures and haven't executed any other draw call in between
		context->renderer->reset_object();

		for (unsigned int i = 0; i < 6; ++i)
		{
			matrices_data.view = captureViews[i];
			hogl_ubo_data(matrices_ubo, &matrices_data, 0, 0);

			cubemap_target->bind();

			// Since we are rendering into a cube map we will need to change the internal fob target to each cube map side
			cubemap_target->set_mip(mip);
			cubemap_target->set_cslot(i);

			// Begin frame
			context->renderer->begin_frame();			

			// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
			context->renderer->reset_object();
			context->renderer->reset_draw_call();
			context->renderer->execute_draw_call(&draw_call);

			// End the frame for the fbo
			context->renderer->end_frame();
			context->renderer->flush();
		}
	}

	// Create irriadiance cube map
	brdfLUTTexture = hogl_new_texture()
		.add_texture()
		.alloc(512, 512, hogl_texture_format::RG16F)
		.set_wrap(hogl_wrap_axis::X, hogl_wrap_type::EDGE_CLAMP)
		.set_wrap(hogl_wrap_axis::Y, hogl_wrap_type::EDGE_CLAMP)
		.set_min_filter(hogl_filter_type::LINEAR)
		.set_mag_filter(hogl_filter_type::LINEAR)
		.slot(2)
		.ptr();


	// Change internal render buffer dimensions
	hogl_adjust_rbo(fbo, hogl_rbuffer_format::d24, 512, 512);

	context->renderer->adjust_viewport(512, 512);

	// Change attached texture
	cubemap_target->attach_texture(brdfLUTTexture);
	cubemap_target->map();

	// Create cube render object
	hogl_render_object quadRender;
	quadRender.mesh = quadMesh;
	quadRender.shader = brdfShader;

	hogl_render_draw_call quad_draw_call;
	quad_draw_call.object = &quadRender;
	quad_draw_call.depth_test = hogl_render_depth::LEQUAL;
	quad_draw_call.seamless_cubemap = true;
	quad_draw_call.render_mode = hogl_render_mode::TRIANGLE_STRIP;

	// Reset the object since we just changed the textures and haven't executed any other draw call in between
	context->renderer->reset_object();

	// Begin frame
	context->renderer->begin_frame();

	// Using level 1 abstraction again to render a cube, we don't need any special settings so we can just render as a single object
	context->renderer->reset_object();
	context->renderer->reset_draw_call();
	context->renderer->execute_draw_call(&quad_draw_call);

	// End the frame for the fbo
	context->renderer->end_frame();
	context->renderer->flush();

	// Static shader uniforms
	hogl_m44<float> projection = hogl_perspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);

	// Create render target for the window
	hogl_wnd_render_target* target = new hogl_wnd_render_target(window);

	// Change clear color
	target->set_clear_color(0.2f * 255, 0.3f * 255, 0.3f * 256, 1.0f * 255);

	// Attach to the renderer
	context->renderer->change_target((hogl_i_render_target*)target);

	matrices_data.projection = projection;

	context->renderer->adjust_viewport(1280, 720);

	// Objects
	hogl_render_object iron;
	iron.mesh = sphereMesh;
	iron.shader = pbrShader;
	iron.textures = { irradianceMap, prefilterMap, brdfLUTTexture, ironAlbedoMap, ironNormalMap, ironMetallicMap, ironRoughnessMap, ironAOMap };

	hogl_render_object gold;
	gold.mesh = sphereMesh;
	gold.shader = pbrShader;
	gold.textures = { irradianceMap, prefilterMap, brdfLUTTexture, goldAlbedoMap, goldNormalMap, goldMetallicMap, goldRoughnessMap, goldAOMap };

	hogl_render_object grass;
	grass.mesh = sphereMesh;
	grass.shader = pbrShader;
	grass.textures = { irradianceMap, prefilterMap, brdfLUTTexture, grassAlbedoMap, grassNormalMap, grassMetallicMap, grassRoughnessMap, grassAOMap };

	hogl_render_object plastic;
	plastic.mesh = sphereMesh;
	plastic.shader = pbrShader;
	plastic.textures = { irradianceMap, prefilterMap, brdfLUTTexture, plasticAlbedoMap, plasticNormalMap, plasticMetallicMap, plasticRoughnessMap, plasticAOMap };

	hogl_render_object wall;
	wall.mesh = sphereMesh;
	wall.shader = pbrShader;
	wall.textures = { irradianceMap, prefilterMap, brdfLUTTexture, wallAlbedoMap, wallNormalMap, wallMetallicMap, wallRoughnessMap, wallAOMap };

	hogl_render_object skybox;
	skybox.mesh = cubeMesh;
	skybox.shader = backgroundShader;
	skybox.textures = { envCubemap };

	hogl_render_draw_call sphere_call;
	sphere_call.object = nullptr;
	sphere_call.depth_test = hogl_render_depth::LEQUAL;
	sphere_call.seamless_cubemap = true;
	sphere_call.render_mode = hogl_render_mode::TRIANGLE_STRIP;

	hogl_render_draw_call background_call;
	background_call.object = &skybox;
	background_call.depth_test = hogl_render_depth::LEQUAL;
	background_call.seamless_cubemap = true;

	// Loop until window is closed
	float lastFrame = 0.0f;
	float deltaTime = 0.0f;
	while (window->is_closed == 0)
	{
		// Calculate delta frame
		float currentFrame = hogl_system_clock();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Begin a new frame
		context->renderer->begin_frame();

		// Matrices
		hogl_m44<float> identity = hogl_m44_empty();
		hogl_m44<float> model = hogl_m44_empty();
		hogl_m44<float> view = hogl_look_at(hogl_v3<float>(0.0f, 0.0f, 10.0f), hogl_v3<float>(0.0f, 0.0f, -1.0f), hogl_v3<float>(0.0, 1.0f, 0.0f));

		pbr_data.model = model;
		matrices_data.view = view;
		pbr_data.camPos = hogl_pad_v3(hogl_v3<float>(0.0f, 0.0f, 15.0f));
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		hogl_ubo_data(matrices_ubo, &matrices_data, 0, 0);
		
		// Rusted iron
		sphere_call.object = &iron;
		model = hogl_translate(identity, hogl_v3<float>(-5.0f, 0.0f, 2.0f));
		pbr_data.model = model;
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&sphere_call);
		
		// Gold
		sphere_call.object = &gold;
		model = hogl_translate(identity, hogl_v3<float>(-3.0f, 0.0f, 2.0f));
		pbr_data.model = model;
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&sphere_call);
		
		// Grass
		sphere_call.object = &grass;
		model = hogl_translate(identity, hogl_v3<float>(-1.0f, 0.0f, 2.0f));
		pbr_data.model = model;
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&sphere_call);
		
		// Plastic
		sphere_call.object = &plastic;
		model = hogl_translate(identity, hogl_v3<float>(1.0f, 0.0f, 2.0f));
		pbr_data.model = model;
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&sphere_call);
		
		// Wall
		sphere_call.object = &wall;
		model = hogl_translate(identity, hogl_v3<float>(3.0f, 0.0f, 2.0f));
		pbr_data.model = model;
		hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&sphere_call);
		
		// Lights
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			hogl_v3<float> newPos = lightPositions[i] + hogl_v3<float>(std::sin(hogl_system_clock() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
		
			model = hogl_translate(identity, newPos);
			model = hogl_scale(model, hogl_v3<float>(0.5f, 0.5f, 0.5f));
		
			pbr_data.lightPosition[i] = hogl_pad_v3(newPos);
			pbr_data.lightColor[i] = hogl_pad_v3(lightColors[i]);
			pbr_data.model = model;
			hogl_ubo_data(pbr_data_ubo, &pbr_data, 0, 0);
		
			context->renderer->reset_object();
			context->renderer->reset_draw_call();
			context->renderer->execute_draw_call(&sphere_call);
		}

		// Skybox
		matrices_data.view = view;
		hogl_ubo_data(matrices_ubo, &matrices_data, 0, 0);
		context->renderer->reset_object();
		context->renderer->reset_draw_call();
		context->renderer->execute_draw_call(&background_call);

		// End frame and flush
		context->renderer->end_frame();
		context->renderer->flush();
		
		// Updates window state this includes is_closed flag
		context->window_manager->update();

		// Polls hogl input
		context->input_system->update();
	}

	// Free resources
	delete cubemap_target;

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