#pragma once

#define HOGL_NSPACE_BEGIN namespace hogl {
#define HOGL_NSPACE_END }

#define HOGL_NSPACE_BEGIN_L1 HOGL_NSPACE_BEGIN namespace al1 {
#define HOGL_NSPACE_END_L2 HOGL_NSPACE_END }

#ifdef HOGL_EXPORT
#define HOGL_API __declspec(dllexport)
#else
#define HOGL_API __declspec(dllimport)
#endif

#define HOGL_LOG_ERROR(text) std::cout << "[hogl:error] " << text << "\n"
#define HOGL_LOG_WARN(text) std::cout << "[hogl:warn] " << text << "\n"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl event modifier, representing if any special keys (ctrl, alt, etc.) were pressed during the event
*/
enum class hogl_e_modifier
{
	SHIFT,
	CONTROL,
	ALT,
	SUPER,
	CAPS,
	NUM
};

/**
 * @brief hogl event action, representing the actual action for key events, REPEAT is keyboard event specific
*/
enum class hogl_e_action
{
	DOWN,
	UP,
	REPEAT
};

/**
 * @brief hogl possible vertex buffer object types
*/
enum class hogl_vbo_type
{
	UNSET,
	ARRAY_BUFFER,
	ELEMENT_BUFFER
};

/**
 * @brief hogl possible vertex buffer object usages
*/
enum class hogl_vbo_usage
{
	UNSET,
	STREAM,
	STATIC,
	DYNAMIC
};

/**
 * @brief hogl possible vertex attribute pointer type
*/
enum class hogl_ap_type
{
	FLOAT,
	UINT
};

/**
 * @brief hogl possible vertex attribute pointer count
*/
enum class hogl_ap_count
{
	ONE		= 1,
	TWO		= 2,
	THREE	= 3,
	FOUR	= 4
};

/**
 * @brief hogl possible depth test values
*/
enum class hogl_render_depth
{
	DISABLED,
	ALWAYS,
	NEVER,
	LESS,
	EQUAL,
	LEQUAL,
	GREATER,
	NOTEQUAL,
	GEQUAL
};

/**
 * @brief hogl possible texture formats
*/
enum class hogl_texture_format
{
	NONE,
	RED,
	RGB,
	RGBA,
	R16F,
	RGB16F,
	RGBA16F,
};

/**
 * @brief hogl possible axis for texture wrapping options
*/
enum class hogl_wrap_axis
{
	X,
	Y,
	Z
};

/**
 * @brief hogl possible texture wrapping options
*/
enum class hogl_wrap_type
{
	REPEAT,
	MIRRORED,
	EDGE_CLAMP,
	BORDER_CLAMP
};

/**
 * @brief hogl possible texture filtering options
*/
enum class hogl_filter_type
{
	NEAREST,
	LINEAR,
	NEAREST_MIPMAP_NEAREST,
	NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_NEAREST,
	LINEAR_MIPMAP_LINEAR
};

/**
 * @brief hogl possible formats for a render buffer
*/
enum class hogl_rbuffer_format
{
	d16,
	d24,
	d32F,
	d24_s8,
	d32F_s8,
	si8
};

// Constants

/**
 * @brief This is dependent on hardware, but the OpenGL 3.3 specification says,
 * that this is the minimum amount of slots that any implementation should have
*/
constexpr auto HOGL_MAX_TEXTURE_SLOTS = 32;

// C++ specific Forward declarations
class hogl_cs_input;
class hogl_cs_wndmanager;
class hogl_cs_renderer;
class hogl_cs_loader;

class hogl_i_render_target;
class hogl_wnd_render_target;

class hogl_bldr_mesh;
class hogl_bldr_shader;
class hogl_bldr_ubo;
class hogl_bldr_texture;

struct hogl_wnd;

struct hogl_render_object;
struct hogl_render_draw_call;

struct hogl_mesh;
struct hogl_shader;
struct hogl_ubo_bind_range;
struct hogl_ubo;
struct hogl_shader_layout;
struct hogl_texture;
struct hogl_framebuffer;

template<typename T>
struct hogl_loader_image;

struct hogl_e_interface;
struct hogl_e_mkey;
struct hogl_e_mmove;
struct hogl_e_mscroll;
struct hogl_e_key;

/**
 * @brief Mouse key callback, parameters: user defined pointer, event details
*/
typedef void (*hogl_ecb_mkey)(void*, const hogl_e_mkey*);

/**
 * @brief Mouse move callback, parameters: user defined pointer, event details
*/
typedef void (*hogl_ecb_mmove)(void*, const hogl_e_mmove*);

/**
 * @brief Mouse scroll callback, parameters: user defined pointer, event details
*/
typedef void (*hogl_ecb_mscroll)(void*, const hogl_e_mscroll*);

/**
 * @brief Key callback, parameters: user defined pointer, event details
*/
typedef void (*hogl_ecb_key)(void*, const hogl_e_key*);

HOGL_NSPACE_END
