#pragma once

#define HOGL_NSPACE_BEGIN namespace hogl {
#define HOGL_NSPACE_END }

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
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4
};

// C++ specific Forward declarations
class hogl_cs_input;
class hogl_cs_wndmanager;
class hogl_cs_renderer;
class hogl_i_render_target;
class hogl_wnd_render_target;
class hogl_cs_loader;
class hogl_obj_builder;

struct hogl_wnd;
struct hogl_object;
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
