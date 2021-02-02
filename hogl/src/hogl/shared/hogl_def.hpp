#pragma once

#ifdef __cplusplus
	#ifdef HOGL_CPP_DLL
		#ifdef HOGL_CPP_EXPORT
			#define HOGL_CPP_API __declspec(dllexport)
		#else
			#define HOGL_CPP_API __declspec(dllimport)
		#endif
	#else
		#define HOGL_CPP_API 
	#endif
#else
	#define HOGL_CPP_API
#endif

/**
* Some hogl functionality can be disabled by passing preprocessor flags, by default all of functionality is enabled
* these are all the flags that can be passed:
*
* HOGL_SUPPRESS_ENTITY_CHECKS				Suppresses all entt error logging
* HOGL_DISABLE_DEBUG						Disables asserts, and other debugging functionality
*/

namespace hogl {
	constexpr size_t MAX_TEXTURE_UNITS = 16;
}
