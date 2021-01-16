/**
* @brief hogl def file, this is used to setup defines, types, and shared functionality
*/

#ifndef _HOGL_DEF_
#define _HOGL_DEF_

// HOGL_API is used to allow C++ compilers to use this library without extra work
#ifndef HOGL_DEVELOPMENT
	// If C++ compiler setup C linkage
	#ifdef __cplusplus
		#ifdef HOGL_EXPORT
			#define HOGL_API extern "C" __declspec(dllexport)
		#else
			#define HOGL_API extern "C" __declspec(dllimport)
		#endif
	#else
		#define HOGL_API
	#endif
#else
	#define HOGL_API
#endif

#ifdef HOGL_SUITE_FULL
	#define HOGL_SUITE_GRAPHICS
	#define HOGL_SUITE_VF
	#define HOGL_SUITE_AUDIO
#endif

#ifndef NULL
	#define NULL 0
#endif

#define hogl_gl_check() if (__hogl_gl_check() != 0) { return HOGL_ERROR_OPENGL_GENERIC; }
#define hogl_al_check() if (__hogl_al_check() != 0) { return HOGL_ERROR_OPENAL_GENERIC; }

/**
* Enums
*/

/**
 * @brief hogl_error enum is used to see if there was an error while performing a function.
 * If the value is not equal to HOGL_ERROR_NONE, that means there was an error in the last call.
 * Additional information will be outputted into the screen automatically.
*/
typedef enum {
	HOGL_ERROR_NONE,
	HOGL_ERROR_MEMORY,
	HOGL_ERROR_BAD_PATH,
	HOGL_ERROR_BAD_READ,
	HOGL_ERROR_BAD_WRITE,
	HOGL_ERROR_BAD_ARGUMENT,
	HOGL_ERROR_OUT_OF_RANGE,
	HOGL_ERROR_ALREADY_ALLOCATED,
	HOGL_ERROR_GLFW_INIT,
	HOGL_ERROR_GLAD_INIT,
	HOGL_ERROR_WND_CREATE,
	HOGL_ERROR_SHADER_COMPILE,
	HOGL_ERROR_SHADER_LINK,
	HOGL_ERROR_UNIFORM_UNKNOWN,
	HOGL_ERROR_FBO_INCOMPLETE,
	HOGL_ERROR_UNKNOWN_MODE,
	HOGL_ERROR_ENDIAN_MISMATCH,
	HOGL_ERROR_VF_BAD_NAME,
	HOGL_ERROR_VF_VFI_MAP,
	HOGL_ERROR_OPENAL_DEVICE,
	HOGL_ERROR_OPENAL_CONTEXT,
	HOGL_ERROR_OPENGL_GENERIC,
	HOGL_ERROR_OPENAL_GENERIC
} hogl_error;

/**
 * @brief hogl possible vertex buffer object types
*/
typedef enum {
	HOGL_VBOT_ARRAY_BUFFER,
	HOGL_VBOT_ELEMENT_BUFFER
} hogl_vbo_type;

/**
 * @brief hogl possible vertex buffer object usages
*/
typedef enum {
	HOGL_VBOU_STREAM,
	HOGL_VBOU_STATIC,
	HOGL_VBOU_DYNAMIC
} hogl_vbo_usage;

/**
 * @brief hogl possible internal element type
*/
typedef enum {
	HOGL_ET_FLOAT,
	HOGL_ET_UINT,
	HOGL_ET_UBYTE
} hogl_element_type;

/**
 * @brief hogl possible depth test values
*/
typedef enum {
	HOGL_RD_DISABLED,
	HOGL_RD_ALWAYS,
	HOGL_RD_NEVER,
	HOGL_RD_LESS,
	HOGL_RD_EQUAL,
	HOGL_RD_LEQUAL,
	HOGL_RD_GREATER,
	HOGL_RD_NOTEQUAL,
	HOGL_RD_GEQUAL
} hogl_render_depth;

/**
 * @brief hogl possible texture formats
*/
typedef enum {
	HOGL_TF_NONE,
	HOGL_TF_RED,
	HOGL_TF_RG,
	HOGL_TF_RGB,
	HOGL_TF_RGBA,
	HOGL_TF_R16F,
	HOGL_TF_RG16F,
	HOGL_TF_RGB16F,
	HOGL_TF_RGBA16F,
} hogl_texture_format;

/**
 * @brief hogl possible axis for texture wrapping options
*/
typedef enum {
	HOGL_WA_X,
	HOGL_WA_Y,
	HOGL_WA_Z
} hogl_wrap_axis;

/**
 * @brief hogl possible texture wrapping options
*/
typedef enum {
	HOGL_WT_DONT_WRAP,
	HOGL_WT_REPEAT,
	HOGL_WT_MIRRORED,
	HOGL_WT_EDGE_CLAMP,
	HOGL_WT_BORDER_CLAMP
} hogl_wrap_type;

/**
 * @brief hogl possible texture filtering options
*/
typedef enum {
	HOGL_FT_NONE,
	HOGL_FT_NEAREST,					// Valid for mag filter
	HOGL_FT_LINEAR,						// Valid for mag filter
	HOGL_FT_NEAREST_MIPMAP_NEAREST,
	HOGL_FT_NEAREST_MIPMAP_LINEAR,
	HOGL_FT_LINEAR_MIPMAP_NEAREST,
	HOGL_FT_LINEAR_MIPMAP_LINEAR
} hogl_filter_type;

/**
 * @brief hogl possible formats for a render buffer
*/
typedef enum {
	HOGL_RBF_d16,
	HOGL_RBF_d24,
	HOGL_RBF_d32F,
	HOGL_RBF_d24_s8,
	HOGL_RBF_d32F_s8,
	HOGL_RBF_si8
} hogl_rbuffer_format;

/**
 * @brief hogl supported render modes
*/
typedef enum {
	HOGL_RM_TRIANGLES,
	HOGL_RM_TRIANGLE_STRIP
} hogl_render_mode;

/**
 * @brief Cubemap side
*/
typedef enum {
	// Positive x
	HOGL_CMS_RIGHT,
	// Negative x
	HOGL_CMS_LEFT,
	// Positive y
	HOGL_CMS_TOP,
	// Negative y
	HOGL_CMS_BOTTOM,
	// Positive z
	HOGL_CMS_BACK,
	// Negative z
	HOGL_CMS_FRONT,
} hogl_cm_side;

/**
 * @brief Audio file playback format
*/
typedef enum {
	HOGL_AF_MONO8,
	HOGL_AF_MONO16,
	HOGL_AF_STEREO8,
	HOGL_AF_STEREO16,
} hogl_audio_format;

/**
 * @brief Checks if any errors occurred in OpenGL since last call
*/
HOGL_API int __hogl_gl_check(void);

/**
 * @brief Checks if any errors occurred in OpenAL since last call
*/
HOGL_API int __hogl_al_check(void);

#endif