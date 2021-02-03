#include "hogl/engine/object_storage.hpp"
#include "hogl/engine/ubo_manager.hpp"
#include "hogl/io/asset_manager.hpp"
#include "hogl/core/framebuffer.hpp"
#include "hogl/core/ubo.hpp"
#include "hogl/entity/mesh.hpp"
#include "hogl/entity/material.hpp"
#include "hogl/entity/camera.hpp"
#include "hogl/entity/scene.hpp"
#include "hogl/entity/components.hpp"

#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class prefilter_ubo : public hogl::ubo {
public:
    struct data {
        float roughness;
    } data;

    prefilter_ubo()
        : ubo("prefilter", 4, sizeof(data))
    {
        p_iBuff = &data;
    }
};

class pbr_ubo : public hogl::ubo {
public:
    struct data {
        glm::mat4 model;

        // UBO requires alignment multiple of 16
        float lightPosition[4][4];
        float lightColor[4][4];

        float camPos[4];
    } data;

    pbr_ubo()
        : ubo("pbr_data", 5, sizeof(data))
    {
        p_iBuff = &data;
    }
};

typedef struct _wavfh
{
    char ChunkId[4];
    unsigned long ChunkSize;
    char FormatTag[4];
    char SubChunkId[4];
    unsigned long SubChunkSize;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned long SampleRate;
    unsigned long BytesPerSecond;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    char DataChunkId[4];
    unsigned long DataSize;
} wavfh;

const unsigned int X_SEGMENTS = 64;
const unsigned int Y_SEGMENTS = 64;
const float PI = 3.14159265359;

hogl::object_storage* storage = nullptr;
hogl::ubo_manager* ubo_manager = nullptr;
hogl::asset_manager* asset_manager = nullptr;

hogl::camera scene_camera(45.0f, 0.1f, 100.0f, 1280.0f, 720.0f);
hogl::camera gen_camera(90.0f, 0.1f, 10.0f, 1.0f, 1.0f);

hogl::scene* scene = nullptr;

hogl::relay_ptr<pbr_ubo> pbrUBO;
hogl::relay_ptr<prefilter_ubo> prefilterUBO;

hogl::relay_ptr<hogl::mesh> cubeMesh;
hogl::relay_ptr<hogl::mesh> quadMesh;
hogl::relay_ptr<hogl::mesh> sphereMesh;

hogl_shader* equirectangularToCubemapShader;
hogl_shader* irradianceShader;
hogl_shader* prefilterShader;
hogl_shader* brdfShader;
hogl_shader* backgroundShader;

hogl::relay_ptr<hogl::framebuffer> fbo;
hogl_renderbuffer* rbo;

// Albedo       slot 3
// Normal       slot 4
// Metallic     slot 5
// Roughness    slot 6
// AO           slot 7

hogl::relay_ptr<hogl::material> iron;

hogl_texture* goldAlbedoMap;
hogl_texture* goldNormalMap;
hogl_texture* goldMetallicMap;
hogl_texture* goldRoughnessMap;
hogl_texture* goldAOMap;

hogl_texture* grassAlbedoMap;
hogl_texture* grassNormalMap;
hogl_texture* grassMetallicMap;
hogl_texture* grassRoughnessMap;
hogl_texture* grassAOMap;

hogl_texture* plasticAlbedoMap;
hogl_texture* plasticNormalMap;
hogl_texture* plasticMetallicMap;
hogl_texture* plasticRoughnessMap;
hogl_texture* plasticAOMap;

hogl_texture* wallAlbedoMap;
hogl_texture* wallNormalMap;
hogl_texture* wallMetallicMap;
hogl_texture* wallRoughnessMap;
hogl_texture* wallAOMap;

hogl_texture* hdr;

hogl::ref<hogl::texture> brdfLUTTexture;
hogl::ref<hogl::cubemap> irradianceMap;
hogl::ref<hogl::cubemap> prefilterMap;
hogl::ref<hogl::cubemap> envCubemap;

hogl_asource* audioSource;
hogl_abuffer* audioBuffer;

float audio_y = 0.0f;

float* generate_sphere_vertices(void) {
    float* result = NULL;
    result = (float*)hogl_malloc((3 + 2 + 3) * (Y_SEGMENTS + 1) * (X_SEGMENTS + 1) * sizeof(float));

    unsigned int offset = 0;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = cos(ySegment * PI);
            float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

            result[offset++] = xPos;
            result[offset++] = yPos;
            result[offset++] = zPos;
            result[offset++] = xSegment;
            result[offset++] = ySegment;
            result[offset++] = xPos;
            result[offset++] = yPos;
            result[offset++] = zPos;
        }
    }

    return result;
}

unsigned int* generate_sphere_indices(void) {
    unsigned int indexCount = 0;
    unsigned int* result = NULL;
    result = (unsigned int*)hogl_malloc(2 * Y_SEGMENTS * (X_SEGMENTS + 1) * sizeof(unsigned int));

    unsigned int offset = 0;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (y % 2 == 0) // even rows
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                result[offset++] = y * (X_SEGMENTS + 1) + x;
                result[offset++] = (y + 1) * (X_SEGMENTS + 1) + x;
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                result[offset++] = (y + 1) * (X_SEGMENTS + 1) + x;
                result[offset++] = y * (X_SEGMENTS + 1) + x;
            }
        }
    }

    return result;
}

void generate_basic_geometry(void) {
    // Data
    float cube_vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };

    float quad_vertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    float* sphere_vertices = NULL;
    unsigned int* sphere_indices = NULL;

    hogl_vbo_desc descs[2];
    hogl_ap_desc apdescs[3];

    // Cube
    descs[0].data = cube_vertices;
    descs[0].data_size = sizeof(cube_vertices);
    descs[0].type = HOGL_VBOT_ARRAY_BUFFER;
    descs[0].usage = HOGL_VBOU_STATIC;
    descs[0].ap_desc = apdescs;
    descs[0].desc_size = 3;
    descs[0].stride = 8 * sizeof(float);

    apdescs[0].divisor = 0;
    apdescs[0].ecount = 3;
    apdescs[0].index = 0;
    apdescs[0].normalized = false;
    apdescs[0].offset = 0;
    apdescs[0].type = HOGL_ET_FLOAT;

    apdescs[1].divisor = 0;
    apdescs[1].ecount = 3;
    apdescs[1].index = 1;
    apdescs[1].normalized = false;
    apdescs[1].offset = 3 * sizeof(float);
    apdescs[1].type = HOGL_ET_FLOAT;

    apdescs[2].divisor = 0;
    apdescs[2].ecount = 2;
    apdescs[2].index = 2;
    apdescs[2].normalized = false;
    apdescs[2].offset = 6 * sizeof(float);
    apdescs[2].type = HOGL_ET_FLOAT;

    cubeMesh = storage->create_new<hogl::mesh>().relay();
    cubeMesh->add_buffer(storage->create_new<hogl::gpu_buffer>(descs[0]));
    cubeMesh->set_render_mode(HOGL_RM_TRIANGLES);

    // Quad
    descs[0].data = quad_vertices;
    descs[0].data_size = sizeof(quad_vertices);
    descs[0].type = HOGL_VBOT_ARRAY_BUFFER;
    descs[0].usage = HOGL_VBOU_STATIC;
    descs[0].ap_desc = apdescs;
    descs[0].desc_size = 2;
    descs[0].stride = 5 * sizeof(float);

    apdescs[0].divisor = 0;
    apdescs[0].ecount = 3;
    apdescs[0].index = 0;
    apdescs[0].normalized = false;
    apdescs[0].offset = 0;
    apdescs[0].type = HOGL_ET_FLOAT;

    apdescs[1].divisor = 0;
    apdescs[1].ecount = 2;
    apdescs[1].index = 1;
    apdescs[1].normalized = false;
    apdescs[1].offset = 3 * sizeof(float);
    apdescs[1].type = HOGL_ET_FLOAT;

    quadMesh = storage->create_new<hogl::mesh>().relay();
    quadMesh->add_buffer(storage->create_new<hogl::gpu_buffer>(descs[0]));
    quadMesh->set_render_mode(HOGL_RM_TRIANGLE_STRIP);

    // Sphere
    sphere_vertices = generate_sphere_vertices();
    sphere_indices = generate_sphere_indices();

    descs[0].data = sphere_vertices;
    descs[0].data_size = (3 + 2 + 3) * (Y_SEGMENTS + 1) * (X_SEGMENTS + 1) * sizeof(float);
    descs[0].type = HOGL_VBOT_ARRAY_BUFFER;
    descs[0].usage = HOGL_VBOU_STATIC;
    descs[0].ap_desc = apdescs;
    descs[0].desc_size = 3;
    descs[0].stride = 8 * sizeof(float);

    descs[1].data = sphere_indices;
    descs[1].data_size = 2 * Y_SEGMENTS * (X_SEGMENTS + 1) * sizeof(unsigned int);
    descs[1].type = HOGL_VBOT_ELEMENT_BUFFER;
    descs[1].usage = HOGL_VBOU_STATIC;
    descs[1].ap_desc = NULL;
    descs[1].desc_size = 0;
    descs[1].stride = sizeof(unsigned int);

    apdescs[0].divisor = 0;
    apdescs[0].ecount = 3;
    apdescs[0].index = 0;
    apdescs[0].normalized = false;
    apdescs[0].offset = 0;
    apdescs[0].type = HOGL_ET_FLOAT;

    apdescs[1].divisor = 0;
    apdescs[1].ecount = 2;
    apdescs[1].index = 1;
    apdescs[1].normalized = false;
    apdescs[1].offset = 3 * sizeof(float);
    apdescs[1].type = HOGL_ET_FLOAT;

    apdescs[2].divisor = 0;
    apdescs[2].ecount = 3;
    apdescs[2].index = 2;
    apdescs[2].normalized = false;
    apdescs[2].offset = 5 * sizeof(float);
    apdescs[2].type = HOGL_ET_FLOAT;

    sphereMesh = storage->create_new<hogl::mesh>().relay();
    sphereMesh->add_buffer(storage->create_new<hogl::gpu_buffer>(descs[0]));
    sphereMesh->add_buffer(storage->create_new<hogl::gpu_buffer>(descs[1]));
    sphereMesh->set_render_mode(HOGL_RM_TRIANGLE_STRIP);

    hogl_free(sphere_vertices);
    hogl_free(sphere_indices);
}

void create_ubos(void) {
    prefilterUBO = storage->create_new<prefilter_ubo>().relay();
    pbrUBO = storage->create_new<pbr_ubo>().relay();
}

void load_shaders(void) {
    hogl_shader_desc desc;

    std::string vertexSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.pbr.vs");
    std::string fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.pbr.fs");

    desc.vertex_source = vertexSrc.c_str();
    desc.fragment_source = fragSrc.c_str();

    hogl::ref<hogl::shader> pbrShader = storage->create_new<hogl::shader>(desc);
    pbrShader->bind();

    pbrShader->sampler_location("irradianceMap", 0);
    pbrShader->sampler_location("prefilterMap", 1);
    pbrShader->sampler_location("brdfLUT", 2);
    pbrShader->sampler_location("albedoMap", 3);
    pbrShader->sampler_location("normalMap", 4);
    pbrShader->sampler_location("metallicMap", 5);
    pbrShader->sampler_location("roughnessMap", 6);
    pbrShader->sampler_location("aoMap", 7);
    pbrShader->ubo_attach(pbrUBO.as<hogl::ubo>());

    ubo_manager->std_pkg()->attach_to(pbrShader.relay());

    vertexSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.cubemap.vs");
    desc.vertex_source = vertexSrc.c_str();

    fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.equirectangular_to_cubemap.fs");
    desc.fragment_source = fragSrc.c_str();
    hogl_shader_new(&equirectangularToCubemapShader, desc);
    hogl_shader_bind(equirectangularToCubemapShader);
    hogl_shader_sampler_location(equirectangularToCubemapShader, "equirectangularMap", 0);
    hogl_shader_ubo_binding(equirectangularToCubemapShader, "per_pass", 0);

    fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.prefilter.fs");
    desc.fragment_source = fragSrc.c_str();
    hogl_shader_new(&prefilterShader, desc);
    hogl_shader_bind(prefilterShader);
    hogl_shader_sampler_location(prefilterShader, "environmentMap", 0);
    hogl_shader_ubo_binding(prefilterShader, "prefilter", 4);
    hogl_shader_ubo_binding(equirectangularToCubemapShader, "per_pass", 0);

    fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.irradiance_convolution.fs");
    desc.fragment_source = fragSrc.c_str();
    hogl_shader_new(&irradianceShader, desc);
    hogl_shader_bind(irradianceShader);
    hogl_shader_sampler_location(irradianceShader, "environmentMap", 0);

    vertexSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.brdf.vs");
    desc.vertex_source = vertexSrc.c_str();
    fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.brdf.fs");
    desc.fragment_source = fragSrc.c_str();
    hogl_shader_new(&brdfShader, desc);

    vertexSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.background.vs");
    desc.vertex_source = vertexSrc.c_str();
    fragSrc = asset_manager->load_txt("res/shaders/pbr/2.2.2.background.fs");
    desc.fragment_source = fragSrc.c_str();
    hogl_shader_new(&backgroundShader, desc);
    hogl_shader_bind(backgroundShader);
    hogl_shader_sampler_location(backgroundShader, "environmentMap", 0);
    hogl_shader_ubo_binding(equirectangularToCubemapShader, "per_pass", 0);

    iron->set_shader(pbrShader);
}

hogl::ref<hogl::texture> load_image(const char* file) {
    hogl_texture_desc desc;
    hogl_texture_data data;
    int components;

    desc.xwrap = HOGL_WT_REPEAT;
    desc.ywrap = HOGL_WT_REPEAT;
    desc.zwrap = HOGL_WT_DONT_WRAP;

    desc.mag_filter = HOGL_FT_LINEAR;
    desc.min_filter = HOGL_FT_LINEAR_MIPMAP_LINEAR;

    data.data = stbi_load(file, (int*)&data.width, (int*)&data.height, &components, 0);

    if (data.data == NULL) {
        hogl_log_error("FAILED TO LOAD FILE %s", file);
    }

    if (components == 1)
    {
        data.data_format = HOGL_TF_RED;
        data.display_format = HOGL_TF_RED;
    }
    else if (components == 3)
    {
        data.data_format = HOGL_TF_RGB;
        data.display_format = HOGL_TF_RGB;
    }
    else if (components == 4)
    {
        data.data_format = HOGL_TF_RGBA;
        data.display_format = HOGL_TF_RGBA;
    }

    data.etype = HOGL_ET_UBYTE;

    hogl::ref<hogl::texture> texture = storage->create_new<hogl::texture>(desc);
    texture->set_data(data);
    texture->gen_mipmap();

    stbi_image_free(data.data);

    return texture;
}

void load_hdr(hogl_texture** texture, const char* file) {
    hogl_texture_desc desc;
    hogl_texture_data data;
    int components;

    stbi_set_flip_vertically_on_load(true);

    desc.xwrap = HOGL_WT_EDGE_CLAMP;
    desc.ywrap = HOGL_WT_EDGE_CLAMP;
    desc.zwrap = HOGL_WT_DONT_WRAP;

    desc.mag_filter = HOGL_FT_LINEAR;
    desc.min_filter = HOGL_FT_LINEAR;

    data.data = stbi_loadf(file, (int*)&data.width, (int*)&data.height, &components, 0);

    if (components == 1)
    {
        data.data_format = HOGL_TF_R16F;
        data.display_format = HOGL_TF_RED;
    }
    else if (components == 3)
    {
        data.data_format = HOGL_TF_RGB16F;
        data.display_format = HOGL_TF_RGB;
    }
    else if (components == 4)
    {
        data.data_format = HOGL_TF_RGBA16F;
        data.display_format = HOGL_TF_RGBA;
    }

    data.etype = HOGL_ET_FLOAT;

    hogl_texture_new(texture, desc);
    hogl_set_texture_data(*texture, &data);
    hogl_texture_gen_mipmap(*texture);

    stbi_image_free(data.data);
}

void load_textures(void) {
    hogl_texture_desc desc;
    hogl_texture_data data;

    //load_image(&ironAlbedoMap, "res/pbr/rusted_iron/albedo.png");
    //load_image(&ironNormalMap, "res/pbr/rusted_iron/normal.png");
    //load_image(&ironMetallicMap, "res/pbr/rusted_iron/metallic.png");
    //load_image(&ironRoughnessMap, "res/pbr/rusted_iron/roughness.png");
    //load_image(&ironAOMap, "res/pbr/rusted_iron/ao.png");

    iron->set_texture(load_image("res/pbr/gold/albedo.png"), 3);
    iron->set_texture(load_image("res/pbr/gold/normal.png"), 4);
    iron->set_texture(load_image("res/pbr/gold/metallic.png"), 5);
    iron->set_texture(load_image("res/pbr/gold/roughness.png"), 6);
    iron->set_texture(load_image("res/pbr/gold/ao.png"), 7);

    //load_image(&goldAlbedoMap, "res/pbr/gold/albedo.png");
    //load_image(&goldNormalMap, "res/pbr/gold/normal.png");
    //load_image(&goldMetallicMap, "res/pbr/gold/metallic.png");
    //load_image(&goldRoughnessMap, "res/pbr/gold/roughness.png");
    //load_image(&goldAOMap, "res/pbr/gold/ao.png");

    //load_image(&grassAlbedoMap, "res/pbr/grass/albedo.png");
    //load_image(&grassNormalMap, "res/pbr/grass/normal.png");
    //load_image(&grassMetallicMap, "res/pbr/grass/metallic.png");
    //load_image(&grassRoughnessMap, "res/pbr/grass/roughness.png");
    //load_image(&grassAOMap, "res/pbr/grass/ao.png");

    //load_image(&plasticAlbedoMap, "res/pbr/plastic/albedo.png");
    //load_image(&plasticNormalMap, "res/pbr/plastic/normal.png");
    //load_image(&plasticMetallicMap, "res/pbr/plastic/metallic.png");
    //load_image(&plasticRoughnessMap, "res/pbr/plastic/roughness.png");
    //load_image(&plasticAOMap, "res/pbr/plastic/ao.png");

    //load_image(&wallAlbedoMap, "res/pbr/wall/albedo.png");
    //load_image(&wallNormalMap, "res/pbr/wall/normal.png");
    //load_image(&wallMetallicMap, "res/pbr/wall/metallic.png");
    //load_image(&wallRoughnessMap, "res/pbr/wall/roughness.png");
    //load_image(&wallAOMap, "res/pbr/wall/ao.png");

    load_hdr(&hdr, "res/hdr/newport_loft.hdr");
    //load_hdr(&hdr, "res/hdr/dikhololo_night_4k.hdr");
    //load_hdr(&hdr, "res/hdr/herkulessaulen_4k.hdr");

    desc.min_filter = HOGL_FT_LINEAR_MIPMAP_LINEAR;
    desc.mag_filter = HOGL_FT_LINEAR;

    desc.xwrap = HOGL_WT_EDGE_CLAMP;
    desc.ywrap = HOGL_WT_EDGE_CLAMP;
    desc.zwrap = HOGL_WT_EDGE_CLAMP;

    data.data = NULL;
    data.height = 512;
    data.width = 512;
    data.etype = HOGL_ET_FLOAT;
    data.data_format = HOGL_TF_RGB16F;
    data.display_format = HOGL_TF_RGB;

    envCubemap = storage->create_new<hogl::cubemap>(desc);

    for (int i = 0; i < 6; i++) {
        envCubemap->set_side((hogl_cm_side)i);
        envCubemap->set_data(data);
    }

    desc.min_filter = HOGL_FT_LINEAR;

    data.height = 32;
    data.width = 32;

    irradianceMap = storage->create_new<hogl::cubemap>(desc);

    for (int i = 0; i < 6; i++) {
        irradianceMap->set_side((hogl_cm_side)i);
        irradianceMap->set_data(data);
    }

    desc.min_filter = HOGL_FT_LINEAR_MIPMAP_LINEAR;

    data.height = 128;
    data.width = 128;

    prefilterMap = storage->create_new<hogl::cubemap>(desc);

    for (int i = 0; i < 6; i++) {
        prefilterMap->set_side((hogl_cm_side)i);
        prefilterMap->set_data(data);
    }

    prefilterMap->gen_mipmap();

    desc.min_filter = HOGL_FT_LINEAR;

    data.height = 512;
    data.width = 512;

    data.display_format = HOGL_TF_RG;
    data.data_format = HOGL_TF_RG16F;

    brdfLUTTexture = storage->create_new<hogl::texture>(desc);
    brdfLUTTexture->set_data(data);
}

void setup_fbos(void) {
    hogl_framebuffer_desc desc;
    hogl_renderbuffer_new(&rbo, HOGL_RBF_d24, 512, 512);

    desc.width = 512;
    desc.height = 512;
    desc.color_attachments = NULL;
    desc.ca_size = 0;

    desc.render_attachments = &rbo;
    desc.ra_size = 1;

    fbo = storage->create_new<hogl::framebuffer>(desc).relay();
}

void load_audio(hogl_abuffer_desc* desc) {
    int error;
    wavfh header;
    FILE* filePtr;
    unsigned int count;

    // Open the wave file in binary.
    error = fopen_s(&filePtr, "res/example_audio_mono.wav", "rb");
    if (error != 0) {
        return;
    }

    // Read in the wave file header.
    count = fread(
        &header,
        sizeof(wavfh),
        1,
        filePtr);
    if (count != 1) {
        return;
    }

    // Reserve data
    desc->data = hogl_malloc(header.DataSize);
    desc->data_size = header.DataSize;
    desc->sample_rate = header.SampleRate;

    // Seek to the start of the contents
    fseek(filePtr, sizeof(wavfh), SEEK_SET);

    // Read buffer
    count = fread(
        desc->data,
        1, // unsigned char
        desc->data_size,
        filePtr);

    // Close the file
    fclose(filePtr);

    // Configure numeric format
    if (header.NumChannels == 1)
    {
        desc->format = header.BitsPerSample == 8 ? HOGL_AF_MONO8 : HOGL_AF_MONO16;
    }
    else {
        desc->format = header.BitsPerSample == 8 ? HOGL_AF_STEREO8 : HOGL_AF_STEREO16;
    }
}

void prepare_pbr(void) {
    storage = new hogl::object_storage();
    ubo_manager = new hogl::ubo_manager();
    asset_manager = new hogl::asset_manager();
    scene = new hogl::scene(ubo_manager->std_pkg());
    iron = storage->create_new<hogl::material>().relay();

    glm::mat4 views[6];
    hogl_rstate rstate;
    float listenerOrientation[6];
    hogl_abuffer_desc adesc;
    adesc.data = NULL;

    generate_basic_geometry();
    create_ubos();
    load_shaders();
    load_textures();
    setup_fbos();

    // PROJECTION
    hogl::relay_ptr<hogl::std_ubo_pkg> ubopkg = ubo_manager->std_pkg();
    ubopkg->per_pass->data.projection = gen_camera.compute_projection();

    // View
    gen_camera.reposition({ 0.0f, 0.0f, 0.0f });

    gen_camera.look_at({ 1.0f, 0.0f, 0.0f });
    gen_camera.up({ 0.0f, -1.0f, 0.0f });
    views[0] = gen_camera.compute_view();

    gen_camera.look_at({ -1.0f, 0.0f, 0.0f });
    views[1] = gen_camera.compute_view();
    
    gen_camera.look_at({ 0.0f, 1.0f, 0.0f });
    gen_camera.up({ 0.0f, 0.0f, 1.0f });
    views[2] = gen_camera.compute_view();

    gen_camera.look_at({ 0.0f, -1.0f, 0.0f });
    gen_camera.up({ 0.0f, 0.0f, -1.0f });
    views[3] = gen_camera.compute_view();

    gen_camera.look_at({ 0.0f, 0.0f, 1.0f });
    gen_camera.up({ 0.0f, -1.0f, 0.0f });
    views[4] = gen_camera.compute_view();

    gen_camera.look_at({ 0.0f, 0.0f, -1.0f });
    views[5] = gen_camera.compute_view();

    fbo->bind();
    hogl_renderbuffer_bind(rbo);

    hogl_viewport(512, 512);
    hogl_set_depth_test(HOGL_RD_LEQUAL);
    hogl_seamless_cm(true);

    hogl_shader_bind(equirectangularToCubemapShader);
    hogl_texture_bind(hdr, 0);
    cubeMesh->bind();

    for (int i = 0; i < 6; i++) {
        ubopkg->per_pass->data.view = views[i];
        ubopkg->per_pass->update();

        envCubemap->set_side((hogl_cm_side)i);
        fbo->ca(envCubemap.relay_as<hogl::texture>(), 0, 0);

        hogl_render_clear(0.5f, 0, 0, 0);
        cubeMesh->render();
    }

    envCubemap->gen_mipmap();

    fbo->bind();
    hogl_renderbuffer_resize(rbo, 32, 32);
    hogl_viewport(32, 32);

    hogl_shader_bind(irradianceShader);
    envCubemap->bind(0);

    for (int i = 0; i < 6; i++) {
        ubopkg->per_pass->data.view = views[i];
        ubopkg->per_pass->update();

        irradianceMap->set_side((hogl_cm_side)i);
        fbo->ca(irradianceMap.relay_as<hogl::texture>(), 0, 0);

        hogl_render_clear(0.5f, 0, 0, 0);
        cubeMesh->render();
    }

    hogl_shader_bind(prefilterShader);
    envCubemap->bind(0);

    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipWidth = 128 * pow(0.5, mip);
        unsigned int mipHeight = 128 * pow(0.5, mip);

        hogl_viewport(mipWidth, mipHeight);
        fbo->bind();
        hogl_renderbuffer_resize(rbo, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterUBO->data.roughness = roughness;
        prefilterUBO->update();

        for (unsigned int i = 0; i < 6; ++i) {
            ubopkg->per_pass->data.view = views[i];
            ubopkg->per_pass->update();

            prefilterMap->set_side((hogl_cm_side)i);
            fbo->ca(prefilterMap.relay_as<hogl::texture>(), 0, mip);

            hogl_render_clear(0, 0, 0, 0);
            cubeMesh->render();
        }
    }

    fbo->bind();
    hogl_renderbuffer_resize(rbo, 512, 512);
    hogl_viewport(512, 512);

    fbo->ca(brdfLUTTexture.relay(), 0, 0);
    fbo->bind();

    hogl_shader_bind(brdfShader);
    quadMesh->bind();
    hogl_set_depth_test(HOGL_RD_LEQUAL);

    hogl_render_clear(0, 0, 0, 0);
    quadMesh->render();

    ubopkg->per_pass->data.projection = scene_camera.compute_projection();

    hogl_reset_framebuffer();
    hogl_viewport(1280, 720);

    hogl_memset(&listenerOrientation[0], 0, 6);
    listenerOrientation[2] = 1.0f;
    listenerOrientation[4] = 1.0f;

    hogl_listener_position(0.0f, 0.0f, 1.0f);
    hogl_listener_velocity(0.0f, 0.0f, 0.0f);
    hogl_listener_orientation(&listenerOrientation[0]);

    hogl_source_new(&audioSource);
    hogl_source_pitch(audioSource, 1.0f);
    hogl_source_gain(audioSource, 1.0f);
    hogl_source_position(audioSource, 0.0f, 0.0f, 0.0f);
    hogl_source_velocity(audioSource, 0.0f, 0, 0.0f);
    hogl_source_loop(audioSource, true);

    load_audio(&adesc);
    hogl_abuffer_new(&audioBuffer, adesc);
    hogl_source_buffer(audioSource, audioBuffer);
    hogl_source_play(audioSource);
    hogl_free(adesc.data);

    iron->set_texture(irradianceMap.as<hogl::texture>(), 0);
    iron->set_texture(prefilterMap.as<hogl::texture>(), 1);
    iron->set_texture(brdfLUTTexture, 2);

    scene_camera.reposition({ 0.0f, 0.0f, 10.0f });
    scene_camera.look_at({ 0.0f, 0.0f, -2.0f });
    scene_camera.up({ 0.0f, 1.0f, 0.0f });

    scene->set_camera(scene_camera);

    hogl::entity entity = scene->spawn_entity();
    auto& transform = entity.get_component<hogl::transform_component>();
    transform.position.z = 2.0f;

    entity.add_component<hogl::mesh_component>(sphereMesh);
    entity.add_component<hogl::renderer_component>(iron);
}

void render_pbr(void) {
    glm::mat4 identity(1.0f);
    glm::mat4 model(1.0f);

    pbrUBO->data.lightPosition[0][0] = -10.0f;
    pbrUBO->data.lightColor[0][0] = 300.0f;
    pbrUBO->data.lightPosition[0][1] = 10.0f;
    pbrUBO->data.lightColor[0][1] = 300.0f;
    pbrUBO->data.lightPosition[0][2] = 10.0f;
    pbrUBO->data.lightColor[0][2] = 300.0f;

    pbrUBO->data.lightPosition[1][0] = 10.0f;
    pbrUBO->data.lightColor[1][0] = 300.0f;
    pbrUBO->data.lightPosition[1][1] = 10.0f;
    pbrUBO->data.lightColor[1][1] = 300.0f;
    pbrUBO->data.lightPosition[1][2] = 10.0f;
    pbrUBO->data.lightColor[1][2] = 300.0f;

    pbrUBO->data.lightPosition[2][0] = -10.0f;
    pbrUBO->data.lightColor[2][0] = 300.0f;
    pbrUBO->data.lightPosition[2][1] = -10.0f;
    pbrUBO->data.lightColor[2][1] = 300.0f;
    pbrUBO->data.lightPosition[2][2] = 10.0f;
    pbrUBO->data.lightColor[2][2] = 300.0f;

    pbrUBO->data.lightPosition[3][0] = 10.0f;
    pbrUBO->data.lightColor[3][0] = 300.0f;
    pbrUBO->data.lightPosition[3][1] = -10.0f;
    pbrUBO->data.lightColor[3][1] = 300.0f;
    pbrUBO->data.lightPosition[3][2] = 10.0f;
    pbrUBO->data.lightColor[3][2] = 300.0f;

    hogl_render_clear(0.2f * 255, 0.3f * 255, 0.3f * 255, 1.0f * 255);

    pbrUBO->data.model = model;
    pbrUBO->data.camPos[0] = 0.0f;
    pbrUBO->data.camPos[1] = 0.0f;
    pbrUBO->data.camPos[2] = 15.0f;

    pbrUBO->update();

    scene->render();

    // Skybox
    cubeMesh->bind();
    hogl_shader_bind(backgroundShader);
    envCubemap->bind(0);
    cubeMesh->render();

    //hogl_source_position(audioSource, 0.0f, audio_y, 0.0f);
    hogl_listener_position(0.0f, audio_y, 0.0f);
    audio_y += 0.5f;
}

void pbr_free(void) {
    hogl_shader_free(equirectangularToCubemapShader);
    hogl_shader_free(irradianceShader);
    hogl_shader_free(prefilterShader);
    hogl_shader_free(brdfShader);
    hogl_shader_free(backgroundShader);

    hogl_renderbuffer_free(rbo);

    hogl_texture_free(hdr);

    hogl_source_free(audioSource);
    hogl_abuffer_free(audioBuffer);

    delete scene;
    delete storage;
    delete ubo_manager;
    delete asset_manager;
}
