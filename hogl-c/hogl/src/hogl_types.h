#pragma once

// C compatibility
#ifdef __cplusplus
#define HOGL_C_BLOCK_START extern "C" {
#define HOGL_C_BLOCK_END }
#else
#define HOGL_C_BLOCK_START 
#define HOGL_C_BLOCK_END
#endif

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

// Typedefs
typedef enum hogl_e_modifier hogl_e_modifier;
typedef enum hogl_e_action hogl_e_action;

typedef struct _hogl_context_impl hogl_context_impl;
typedef struct hogl_context hogl_context;

typedef struct hogl_wnd hogl_wnd;

typedef struct hogl_e_interface hogl_e_interface;
typedef struct hogl_e_mkey hogl_e_mkey;
typedef struct hogl_e_mmove hogl_e_mmove;
typedef struct hogl_e_mscroll hogl_e_mscroll;
typedef struct hogl_e_key hogl_e_key;

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
