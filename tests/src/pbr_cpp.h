#include "hogl/entity/mesh.hpp"
#include "hogl/entity/material.hpp"
#include "hogl/core/object_storage.hpp"
#include "hogl/core/framebuffer.hpp"
#include "hogl/io/asset_manager.hpp"
#include "hogl/entity/camera.hpp"
#include "hogl/entity/scene.hpp"
#include "hogl/entity/components.hpp"

#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

typedef struct _prefilter_data
{
    float roughness;
} prefilter_data;

typedef struct _pbr_data
{
    float model[16];

    // UBO requires alignment multiple of 16
    float lightPosition[4][4];
    float lightColor[4][4];

    float camPos[4];
} pbr_data;

typedef struct _matrices_data
{
    float projection[16];
    float view[16];
} matrices_data;

typedef struct _vec3 {
    float x;
    float y;
    float z;
} vec3;

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

hogl::camera scene_camera(45.0f, 0.1f, 100.0f, 1280.0f, 720.0f);
hogl::camera gen_camera(90.0f, 0.1f, 10.0f, 1.0f, 1.0f);

hogl::scene scene;

prefilter_data pfd;
pbr_data pbrd;
matrices_data md;

hogl::relay_ptr<hogl::mesh> cubeMesh;
hogl::relay_ptr<hogl::mesh> quadMesh;
hogl::relay_ptr<hogl::mesh> sphereMesh;

hogl_ubo* pbrDataUBO;
hogl_ubo* matricesUBO;
hogl_ubo* prefilterUBO;

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
    hogl_ubo_desc desc;
    desc.offset = 0;

    desc.bp = 0;
    desc.stride = sizeof(pbr_data);
    hogl_ubo_new(&pbrDataUBO, desc);

    desc.bp = 4;
    desc.stride = sizeof(prefilter_data);
    hogl_ubo_new(&prefilterUBO, desc);

    desc.bp = 7;
    desc.stride = sizeof(matricesUBO);
    hogl_ubo_new(&matricesUBO, desc);
}

void load_shaders(void) {
    hogl_shader_desc desc;

    //desc.vertex_source = "#version 420 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 2) in vec2 aTexCoords;\nlayout (location = 1) in vec3 aNormal;\n\nout vec2 TexCoords;\nout vec3 WorldPos;\nout vec3 Normal;\n\nlayout (std140, binding = 7) uniform matrices\n{\n\tuniform mat4 projection;\n\tuniform mat4 view;\n};\n\nlayout (std140, binding = 5) uniform pbr_data\n{\n    uniform mat4 model;\n\n    // lights\n    uniform vec4 lightPositions[4];\n    uniform vec4 lightColors[4];\n\n    uniform vec4 camPos;\n};\n\nvoid main()\n{\n    TexCoords = aTexCoords;\n    WorldPos = vec3(model * vec4(aPos, 1.0));\n    Normal = mat3(model) * aNormal;   \n\n    gl_Position =  projection * view * vec4(WorldPos, 1.0);\n}";
    desc.vertex_source = "#version 420 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoords;\nlayout (location = 2) in vec3 aNormal;\n\nout vec2 TexCoords;\nout vec3 WorldPos;\nout vec3 Normal;\n\nlayout (std140, binding = 7) uniform matrices\n{\n\tuniform mat4 projection;\n\tuniform mat4 view;\n};\n\nlayout (std140, binding = 5) uniform pbr_data\n{\n    uniform mat4 model;\n\n    // lights\n    uniform vec4 lightPositions[4];\n    uniform vec4 lightColors[4];\n\n    uniform vec4 camPos;\n};\n\nvoid main()\n{\n    TexCoords = aTexCoords;\n    WorldPos = vec3(model * vec4(aPos, 1.0));\n    Normal = mat3(model) * aNormal;   \n\n    gl_Position =  projection * view * vec4(WorldPos, 1.0);\n}";
    desc.fragment_source = "#version 420 core\nout vec4 FragColor;\nin vec2 TexCoords;\nin vec3 WorldPos;\nin vec3 Normal;\n\n// IBL\nuniform samplerCube irradianceMap;\nuniform samplerCube prefilterMap;\nuniform sampler2D brdfLUT;\n\n// material parameters\nuniform sampler2D albedoMap;\nuniform sampler2D normalMap;\nuniform sampler2D metallicMap;\nuniform sampler2D roughnessMap;\nuniform sampler2D aoMap;\n\nlayout (std140, binding = 5) uniform pbr_data\n{\n    uniform mat4 model;\n\n    // lights\n    uniform vec4 lightPositions[4];\n    uniform vec4 lightColors[4];\n\n    uniform vec4 camPos;\n};\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\n// Easy trick to get tangent-normals to world-space to keep PBR code simplified.\n// Don't worry if you don't get what's going on; you generally want to do normal \n// mapping the usual way for performance anways; I do plan make a note of this \n// technique somewhere later in the normal mapping tutorial.\nvec3 getNormalFromMap()\n{\n    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;\n\n    vec3 Q1  = dFdx(WorldPos);\n    vec3 Q2  = dFdy(WorldPos);\n    vec2 st1 = dFdx(TexCoords);\n    vec2 st2 = dFdy(TexCoords);\n\n    vec3 N   = normalize(Normal);\n    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);\n    vec3 B  = -normalize(cross(N, T));\n    mat3 TBN = mat3(T, B, N);\n\n    return normalize(TBN * tangentNormal);\n}\n// ----------------------------------------------------------------------------\nfloat DistributionGGX(vec3 N, vec3 H, float roughness)\n{\n    float a = roughness*roughness;\n    float a2 = a*a;\n    float NdotH = max(dot(N, H), 0.0);\n    float NdotH2 = NdotH*NdotH;\n\n    float nom   = a2;\n    float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n    denom = PI * denom * denom;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySchlickGGX(float NdotV, float roughness)\n{\n    float r = (roughness + 1.0);\n    float k = (r*r) / 8.0;\n\n    float nom   = NdotV;\n    float denom = NdotV * (1.0 - k) + k;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)\n{\n    float NdotV = max(dot(N, V), 0.0);\n    float NdotL = max(dot(N, L), 0.0);\n    float ggx2 = GeometrySchlickGGX(NdotV, roughness);\n    float ggx1 = GeometrySchlickGGX(NdotL, roughness);\n\n    return ggx1 * ggx2;\n}\n// ----------------------------------------------------------------------------\nvec3 fresnelSchlick(float cosTheta, vec3 F0)\n{\n    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);\n}\n// ----------------------------------------------------------------------------\nvec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)\n{\n    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);\n}   \n// ----------------------------------------------------------------------------\nvoid main()\n{\t\t\n    // material properties\n    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));\n    float metallic = texture(metallicMap, TexCoords).r;\n    float roughness = texture(roughnessMap, TexCoords).r;\n    float ao = texture(aoMap, TexCoords).r;\n       \n    // input lighting data\n    vec3 N = getNormalFromMap();\n    vec3 V = normalize(camPos.xyz - WorldPos);\n    vec3 R = reflect(-V, N); \n\n    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 \n    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    \n    vec3 F0 = vec3(0.04); \n    F0 = mix(F0, albedo, metallic);\n\n    // reflectance equation\n    vec3 Lo = vec3(0.0);\n    for(int i = 0; i < 4; ++i) \n    {\n        // calculate per-light radiance\n        vec3 L = normalize(lightPositions[i].xyz - WorldPos);\n        vec3 H = normalize(V + L);\n        float distance = length(lightPositions[i].xyz - WorldPos);\n        float attenuation = 1.0 / (distance * distance);\n        vec3 radiance = lightColors[i].xyz * attenuation;\n\n        // Cook-Torrance BRDF\n        float NDF = DistributionGGX(N, H, roughness);   \n        float G   = GeometrySmith(N, V, L, roughness);    \n        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        \n        \n        vec3 nominator    = NDF * G * F;\n        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.\n        vec3 specular = nominator / denominator;\n        \n         // kS is equal to Fresnel\n        vec3 kS = F;\n        // for energy conservation, the diffuse and specular light can't\n        // be above 1.0 (unless the surface emits light); to preserve this\n        // relationship the diffuse component (kD) should equal 1.0 - kS.\n        vec3 kD = vec3(1.0) - kS;\n        // multiply kD by the inverse metalness such that only non-metals \n        // have diffuse lighting, or a linear blend if partly metal (pure metals\n        // have no diffuse light).\n        kD *= 1.0 - metallic;\t                \n            \n        // scale light by NdotL\n        float NdotL = max(dot(N, L), 0.0);        \n\n        // add to outgoing radiance Lo\n        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again\n    }   \n    \n    // ambient lighting (we now use IBL as the ambient term)\n    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);\n    \n    vec3 kS = F;\n    vec3 kD = 1.0 - kS;\n    kD *= 1.0 - metallic;\t  \n    \n    vec3 irradiance = texture(irradianceMap, N).rgb;\n    vec3 diffuse      = irradiance * albedo;\n    \n    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.\n    const float MAX_REFLECTION_LOD = 4.0;\n    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    \n    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;\n    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);\n\n    vec3 ambient = (kD * diffuse + specular) * ao;\n    \n    vec3 color = ambient + Lo;\n\n    // HDR tonemapping\n    color = color / (color + vec3(1.0));\n    // gamma correct\n    color = pow(color, vec3(1.0/2.2)); \n\n    FragColor = vec4(color , 1.0);\n}\n";
    
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
    pbrShader->ubo_location("pbr_data", 5);
    pbrShader->ubo_location("matrices", 7);

    desc.vertex_source = "#version 420 core\nlayout (location = 0) in vec3 aPos;\n\nout vec3 WorldPos;\n\nlayout (std140, binding = 7) uniform matrices\n{\n\tuniform mat4 projection;\n\tuniform mat4 view;\n};\n\nvoid main()\n{\n    WorldPos = aPos;  \n    gl_Position =  projection * view * vec4(WorldPos, 1.0);\n}";

    desc.fragment_source = "#version 420 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform sampler2D equirectangularMap;\n\nconst vec2 invAtan = vec2(0.1591, 0.3183);\nvec2 SampleSphericalMap(vec3 v)\n{\n    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));\n    uv *= invAtan;\n    uv += 0.5;\n    return uv;\n}\n\nvoid main()\n{\t\t\n    vec2 uv = SampleSphericalMap(normalize(WorldPos));\n    vec3 color = texture(equirectangularMap, uv).rgb;\n    \n    FragColor = vec4(color, 1.0);\n}\n";
    hogl_shader_new(&equirectangularToCubemapShader, desc);
    hogl_shader_bind(equirectangularToCubemapShader);
    hogl_shader_sampler_location(equirectangularToCubemapShader, "equirectangularMap", 0);
    hogl_shader_ubo_binding(equirectangularToCubemapShader, "matrices", 7);

    desc.fragment_source = "#version 420 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform samplerCube environmentMap;\n\nlayout (std140, binding = 4) uniform prefilter\n{\n    uniform float roughness;\n};\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\nfloat DistributionGGX(vec3 N, vec3 H, float roughness)\n{\n    float a = roughness*roughness;\n    float a2 = a*a;\n    float NdotH = max(dot(N, H), 0.0);\n    float NdotH2 = NdotH*NdotH;\n\n    float nom   = a2;\n    float denom = (NdotH2 * (a2 - 1.0) + 1.0);\n    denom = PI * denom * denom;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\n// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html\n// efficient VanDerCorpus calculation.\nfloat RadicalInverse_VdC(uint bits) \n{\n     bits = (bits << 16u) | (bits >> 16u);\n     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n     return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n}\n// ----------------------------------------------------------------------------\nvec2 Hammersley(uint i, uint N)\n{\n\treturn vec2(float(i)/float(N), RadicalInverse_VdC(i));\n}\n// ----------------------------------------------------------------------------\nvec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)\n{\n\tfloat a = roughness*roughness;\n\t\n\tfloat phi = 2.0 * PI * Xi.x;\n\tfloat cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));\n\tfloat sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n\t\n\t// from spherical coordinates to cartesian coordinates - halfway vector\n\tvec3 H;\n\tH.x = cos(phi) * sinTheta;\n\tH.y = sin(phi) * sinTheta;\n\tH.z = cosTheta;\n\t\n\t// from tangent-space H vector to world-space sample vector\n\tvec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\tvec3 tangent   = normalize(cross(up, N));\n\tvec3 bitangent = cross(N, tangent);\n\t\n\tvec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n\treturn normalize(sampleVec);\n}\n// ----------------------------------------------------------------------------\nvoid main()\n{\t\t\n    vec3 N = normalize(WorldPos);\n    \n    // make the simplyfying assumption that V equals R equals the normal \n    vec3 R = N;\n    vec3 V = R;\n\n    const uint SAMPLE_COUNT = 1024u;\n    vec3 prefilteredColor = vec3(0.0);\n    float totalWeight = 0.0;\n    \n    for(uint i = 0u; i < SAMPLE_COUNT; ++i)\n    {\n        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).\n        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n        vec3 H = ImportanceSampleGGX(Xi, N, roughness);\n        vec3 L  = normalize(2.0 * dot(V, H) * H - V);\n\n        float NdotL = max(dot(N, L), 0.0);\n        if(NdotL > 0.0)\n        {\n            // sample from the environment's mip level based on roughness/pdf\n            float D   = DistributionGGX(N, H, roughness);\n            float NdotH = max(dot(N, H), 0.0);\n            float HdotV = max(dot(H, V), 0.0);\n            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; \n\n            float resolution = 512.0; // resolution of source cubemap (per face)\n            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);\n            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);\n\n            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); \n            \n            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;\n            totalWeight      += NdotL;\n        }\n    }\n\n    prefilteredColor = prefilteredColor / totalWeight;\n\n    FragColor = vec4(prefilteredColor, 1.0);\n}\n";
    hogl_shader_new(&prefilterShader, desc);
    hogl_shader_bind(prefilterShader);
    hogl_shader_sampler_location(prefilterShader, "environmentMap", 0);
    hogl_shader_ubo_binding(prefilterShader, "prefilter", 4);
    hogl_shader_ubo_binding(prefilterShader, "matrices", 7);

    desc.fragment_source = "#version 420 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform samplerCube environmentMap;\n\nconst float PI = 3.14159265359;\n\nvoid main()\n{		\n    vec3 N = normalize(WorldPos);\n\n    vec3 irradiance = vec3(0.0);   \n    \n    // tangent space calculation from origin point\n    vec3 up    = vec3(0.0, 1.0, 0.0);\n    vec3 right = cross(up, N);\n    up            = cross(N, right);\n       \n    float sampleDelta = 0.025;\n    float nrSamples = 0.0f;\n    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)\n    {\n        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)\n        {\n            // spherical to cartesian (in tangent space)\n            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));\n            // tangent space to world\n            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; \n\n            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);\n            nrSamples++;\n        }\n    }\n    irradiance = PI * irradiance * (1.0 / float(nrSamples));\n    \n    FragColor = vec4(irradiance, 1.0);\n}\n";
    hogl_shader_new(&irradianceShader, desc);
    hogl_shader_bind(irradianceShader);
    hogl_shader_sampler_location(irradianceShader, "environmentMap", 0);

    desc.vertex_source = "#version 420 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 1) in vec2 aTexCoords;\n\nout vec2 TexCoords;\n\nvoid main()\n{\n    TexCoords = aTexCoords;\n\tgl_Position = vec4(aPos, 1.0);\n}";
    desc.fragment_source = "#version 420 core\nout vec2 FragColor;\nin vec2 TexCoords;\n\nconst float PI = 3.14159265359;\n// ----------------------------------------------------------------------------\n// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html\n// efficient VanDerCorpus calculation.\nfloat RadicalInverse_VdC(uint bits) \n{\n     bits = (bits << 16u) | (bits >> 16u);\n     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);\n     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);\n     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);\n     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);\n     return float(bits) * 2.3283064365386963e-10; // / 0x100000000\n}\n// ----------------------------------------------------------------------------\nvec2 Hammersley(uint i, uint N)\n{\n\treturn vec2(float(i)/float(N), RadicalInverse_VdC(i));\n}\n// ----------------------------------------------------------------------------\nvec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)\n{\n\tfloat a = roughness*roughness;\n\t\n\tfloat phi = 2.0 * PI * Xi.x;\n\tfloat cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));\n\tfloat sinTheta = sqrt(1.0 - cosTheta*cosTheta);\n\t\n\t// from spherical coordinates to cartesian coordinates - halfway vector\n\tvec3 H;\n\tH.x = cos(phi) * sinTheta;\n\tH.y = sin(phi) * sinTheta;\n\tH.z = cosTheta;\n\t\n\t// from tangent-space H vector to world-space sample vector\n\tvec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);\n\tvec3 tangent   = normalize(cross(up, N));\n\tvec3 bitangent = cross(N, tangent);\n\t\n\tvec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;\n\treturn normalize(sampleVec);\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySchlickGGX(float NdotV, float roughness)\n{\n    // note that we use a different k for IBL\n    float a = roughness;\n    float k = (a * a) / 2.0;\n\n    float nom   = NdotV;\n    float denom = NdotV * (1.0 - k) + k;\n\n    return nom / denom;\n}\n// ----------------------------------------------------------------------------\nfloat GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)\n{\n    float NdotV = max(dot(N, V), 0.0);\n    float NdotL = max(dot(N, L), 0.0);\n    float ggx2 = GeometrySchlickGGX(NdotV, roughness);\n    float ggx1 = GeometrySchlickGGX(NdotL, roughness);\n\n    return ggx1 * ggx2;\n}\n// ----------------------------------------------------------------------------\nvec2 IntegrateBRDF(float NdotV, float roughness)\n{\n    vec3 V;\n    V.x = sqrt(1.0 - NdotV*NdotV);\n    V.y = 0.0;\n    V.z = NdotV;\n\n    float A = 0.0;\n    float B = 0.0; \n\n    vec3 N = vec3(0.0, 0.0, 1.0);\n    \n    const uint SAMPLE_COUNT = 1024u;\n    for(uint i = 0u; i < SAMPLE_COUNT; ++i)\n    {\n        // generates a sample vector that's biased towards the\n        // preferred alignment direction (importance sampling).\n        vec2 Xi = Hammersley(i, SAMPLE_COUNT);\n        vec3 H = ImportanceSampleGGX(Xi, N, roughness);\n        vec3 L = normalize(2.0 * dot(V, H) * H - V);\n\n        float NdotL = max(L.z, 0.0);\n        float NdotH = max(H.z, 0.0);\n        float VdotH = max(dot(V, H), 0.0);\n\n        if(NdotL > 0.0)\n        {\n            float G = GeometrySmith(N, V, L, roughness);\n            float G_Vis = (G * VdotH) / (NdotH * NdotV);\n            float Fc = pow(1.0 - VdotH, 5.0);\n\n            A += (1.0 - Fc) * G_Vis;\n            B += Fc * G_Vis;\n        }\n    }\n    A /= float(SAMPLE_COUNT);\n    B /= float(SAMPLE_COUNT);\n    return vec2(A, B);\n}\n// ----------------------------------------------------------------------------\nvoid main() \n{\n    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);\n    FragColor = integratedBRDF;\n}";
    hogl_shader_new(&brdfShader, desc);

    desc.vertex_source = "#version 420 core\nlayout (location = 0) in vec3 aPos;\n\nlayout (std140, binding = 7) uniform matrices\n{\n\tuniform mat4 projection;\n\tuniform mat4 view;\n};\n\nout vec3 WorldPos;\n\nvoid main()\n{\n    WorldPos = aPos;\n\n\tmat4 rotView = mat4(mat3(view));\n\tvec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);\n\n\tgl_Position = clipPos.xyww;\n}";
    desc.fragment_source = "#version 420 core\nout vec4 FragColor;\nin vec3 WorldPos;\n\nuniform samplerCube environmentMap;\n\nvoid main()\n{\t\t\n    vec3 envColor = textureLod(environmentMap, WorldPos, 0.0).rgb;\n    \n    // HDR tonemap and gamma correct\n    envColor = envColor / (envColor + vec3(1.0));\n    envColor = pow(envColor, vec3(1.0/2.2)); \n    \n    FragColor = vec4(envColor, 1.0);\n}\n";
    hogl_shader_new(&backgroundShader, desc);
    hogl_shader_bind(backgroundShader);
    hogl_shader_sampler_location(backgroundShader, "environmentMap", 0);
    hogl_shader_ubo_binding(backgroundShader, "matrices", 7);

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

void setup_ubos(void) {
    hogl_ubo_desc desc;

    desc.bp = 5;
    desc.offset = 0;
    desc.stride = sizeof(pbr_data);

    hogl_ubo_new(&pbrDataUBO, desc);

    desc.bp = 7;
    desc.offset = 0;
    desc.stride = sizeof(matrices_data);

    hogl_ubo_new(&matricesUBO, desc);

    desc.bp = 4;
    desc.offset = 0;
    desc.stride = sizeof(prefilter_data);

    hogl_ubo_new(&prefilterUBO, desc);
}

void normalize(vec3* p) {
    float w = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    p->x /= w;
    p->y /= w;
    p->z /= w;
}

vec3 cross(vec3 l, vec3 r) {
    vec3 ret = { 0 };
    ret.x = l.y * r.z - l.z * r.y;
    ret.y = l.z * r.x - l.x * r.z;
    ret.z = l.x * r.y - l.y * r.x;
    return ret;
}

float dot(vec3 l, vec3 r)
{
    return l.x * r.x + l.y * r.y + l.z * r.z;
}

void translate(vec3 val, float* mret) {
    mret[0 + 3 * 4] = val.x;
    mret[1 + 3 * 4] = val.y;
    mret[2 + 3 * 4] = val.z;
}

void mat_init(float* mret) {
    memset(mret, 0, 16 * sizeof(float));

    mret[0 + 0 * 4] = 1.0f;
    mret[1 + 1 * 4] = 1.0f;
    mret[2 + 2 * 4] = 1.0f;
    mret[3 + 3 * 4] = 1.0f;
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
    iron = storage->create_new<hogl::material>().relay();

    float views[6][16];
    hogl_rstate rstate;
    float listenerOrientation[6];
    hogl_abuffer_desc adesc;
    adesc.data = NULL;

    mat_init(&views[0][0]);
    mat_init(&views[1][0]);
    mat_init(&views[2][0]);
    mat_init(&views[3][0]);
    mat_init(&views[4][0]);
    mat_init(&views[5][0]);

    mat_init(&md.projection[0]);

    generate_basic_geometry();
    setup_ubos();
    load_shaders();
    load_textures();
    setup_fbos();

    // PROJECTION
    hogl_smemcpy(&md.projection[0], glm::value_ptr(gen_camera.compute_projection()), 16 * sizeof(float));

    // View
    gen_camera.reposition({ 0.0f, 0.0f, 0.0f });

    gen_camera.look_at({ 1.0f, 0.0f, 0.0f });
    gen_camera.up({ 0.0f, -1.0f, 0.0f });
    hogl_smemcpy(&views[0][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));

    gen_camera.look_at({ -1.0f, 0.0f, 0.0f });
    hogl_smemcpy(&views[1][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));
    
    gen_camera.look_at({ 0.0f, 1.0f, 0.0f });
    gen_camera.up({ 0.0f, 0.0f, 1.0f });
    hogl_smemcpy(&views[2][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));

    gen_camera.look_at({ 0.0f, -1.0f, 0.0f });
    gen_camera.up({ 0.0f, 0.0f, -1.0f });
    hogl_smemcpy(&views[3][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));

    gen_camera.look_at({ 0.0f, 0.0f, 1.0f });
    gen_camera.up({ 0.0f, -1.0f, 0.0f });
    hogl_smemcpy(&views[4][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));

    gen_camera.look_at({ 0.0f, 0.0f, -1.0f });
    hogl_smemcpy(&views[5][0], glm::value_ptr(gen_camera.compute_view()), 16 * sizeof(float));

    fbo->bind();
    hogl_renderbuffer_bind(rbo);

    hogl_viewport(512, 512);
    hogl_set_depth_test(HOGL_RD_LEQUAL);
    hogl_seamless_cm(true);

    hogl_shader_bind(equirectangularToCubemapShader);
    hogl_texture_bind(hdr, 0);
    cubeMesh->bind();

    for (int i = 0; i < 6; i++) {
        memcpy(&md.view[0], &views[i][0], 16 * sizeof(float));
        hogl_ubo_data(matricesUBO, &md, sizeof(md));

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
        memcpy(&md.view[0], &views[i][0], 16 * sizeof(float));
        hogl_ubo_data(matricesUBO, &md, sizeof(md));

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
        pfd.roughness = roughness;
        hogl_ubo_data(prefilterUBO, &pfd, sizeof(prefilter_data));

        for (unsigned int i = 0; i < 6; ++i) {
            memcpy(&md.view[0], &views[i][0], 16 * sizeof(float));
            hogl_ubo_data(matricesUBO, &md, sizeof(md));

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

    hogl_smemcpy(&md.projection[0], glm::value_ptr(scene_camera.compute_projection()), 16 * sizeof(float));

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

    scene.set_camera(scene_camera);

    hogl::entity entity = scene.spawn_entity();
    auto& transform = entity.get_component<hogl::transform_component>();
    transform.position.z = 2.0f;

    entity.add_component<hogl::mesh_component>(sphereMesh);
    entity.add_component<hogl::renderer_component>(iron);
}

void render_pbr(void) {
    float identity[16];
    float model[16];
    float view[16];

    mat_init(&identity[0]);
    mat_init(&model[0]);
    mat_init(&view[0]);

    vec3 eye = { 0 };
    vec3 target = { 0 };
    vec3 up = { 0 };

    vec3 translateVec = { 0 };

    pbrd.lightPosition[0][0] = -10.0f;
    pbrd.lightColor[0][0] = 300.0f;
    pbrd.lightPosition[0][1] = 10.0f;
    pbrd.lightColor[0][1] = 300.0f;
    pbrd.lightPosition[0][2] = 10.0f;
    pbrd.lightColor[0][2] = 300.0f;

    pbrd.lightPosition[1][0] = 10.0f;
    pbrd.lightColor[1][0] = 300.0f;
    pbrd.lightPosition[1][1] = 10.0f;
    pbrd.lightColor[1][1] = 300.0f;
    pbrd.lightPosition[1][2] = 10.0f;
    pbrd.lightColor[1][2] = 300.0f;

    pbrd.lightPosition[2][0] = -10.0f;
    pbrd.lightColor[2][0] = 300.0f;
    pbrd.lightPosition[2][1] = -10.0f;
    pbrd.lightColor[2][1] = 300.0f;
    pbrd.lightPosition[2][2] = 10.0f;
    pbrd.lightColor[2][2] = 300.0f;

    pbrd.lightPosition[3][0] = 10.0f;
    pbrd.lightColor[3][0] = 300.0f;
    pbrd.lightPosition[3][1] = -10.0f;
    pbrd.lightColor[3][1] = 300.0f;
    pbrd.lightPosition[3][2] = 10.0f;
    pbrd.lightColor[3][2] = 300.0f;

    hogl_render_clear(0.2f * 255, 0.3f * 255, 0.3f * 255, 1.0f * 255);

    hogl_smemcpy(&view[0], glm::value_ptr(scene_camera.compute_view()), 16 * sizeof(float));

    memcpy(&pbrd.model[0], &model[0], 16 * sizeof(float));

    pbrd.camPos[0] = 0.0f;
    pbrd.camPos[1] = 0.0f;
    pbrd.camPos[2] = 15.0f;

    memcpy(&md.view[0], &view[0], 16 * sizeof(float));

    hogl_ubo_data(pbrDataUBO, &pbrd, sizeof(pbr_data));
    hogl_ubo_data(matricesUBO, &md, sizeof(matrices_data));

    // Rusted iron
    translateVec.x = 0.0f;
    translateVec.y = 0.0f;
    translateVec.z = 2.0f;

    translate(translateVec, &model[0]);

    memcpy(&pbrd.model[0], &model[0], 16 * sizeof(float));
    hogl_ubo_data(pbrDataUBO, &pbrd, sizeof(pbr_data));

    scene.render();

    memcpy(&model[0], &identity[0], 16 * sizeof(float));

    // Skybox
    memcpy(&md.view[0], &view[0], sizeof(float) * 16);
    hogl_ubo_data(matricesUBO, &md, sizeof(matrices_data));

    cubeMesh->bind();
    hogl_shader_bind(backgroundShader);
    envCubemap->bind(0);
    cubeMesh->render();

    //hogl_source_position(audioSource, 0.0f, audio_y, 0.0f);
    hogl_listener_position(0.0f, audio_y, 0.0f);
    audio_y += 0.5f;
}

void pbr_free(void) {
    hogl_ubo_free(pbrDataUBO);
    hogl_ubo_free(matricesUBO);
    hogl_ubo_free(prefilterUBO);

    hogl_shader_free(equirectangularToCubemapShader);
    hogl_shader_free(irradianceShader);
    hogl_shader_free(prefilterShader);
    hogl_shader_free(brdfShader);
    hogl_shader_free(backgroundShader);

    hogl_renderbuffer_free(rbo);

    hogl_texture_free(hdr);

    hogl_source_free(audioSource);
    hogl_abuffer_free(audioBuffer);

    delete storage;
}
