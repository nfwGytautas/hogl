#pragma once

#define HOGL_NSPACE_BEGIN namespace hogl {
#define HOGL_NSPACE_END }

HOGL_NSPACE_BEGIN

/**
 * @brief hogl event modifier, representing if any special keys (ctrl, alt, etc.) were pressed during the event
*/
enum hogl_e_modifier
{
	HOGL_ENUM_MOD_SHIFT,
	HOGL_ENUM_MOD_CONTROL,
	HOGL_ENUM_MOD_ALT,
	HOGL_ENUM_MOD_SUPER,
	HOGL_ENUM_MOD_CAPS,
	HOGL_ENUM_MOD_NUM
};

/**
 * @brief hogl event action, representing the actual action for key events, REPEAT is keyboard event specific
*/
enum hogl_e_action
{
	HOGL_ENUM_MOD_DOWN,
	HOGL_ENUM_MOD_UP,
	HOGL_ENUM_MOD_REPEAT
};

// C++ specific Forward declarations
class hogl_cs_input;
class hogl_cs_wndmanager;

struct hogl_wnd;
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
