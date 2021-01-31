#pragma once

#ifdef __cplusplus
	#ifdef HOGL_CPP_EXPORT
		#define HOGL_CPP_API __declspec(dllexport)
	#else
		#define HOGL_CPP_API __declspec(dllimport)
	#endif
#else
	#define HOGL_CPP_API
#endif

namespace hogl {
	constexpr size_t MAX_TEXTURE_UNITS = 16;
}
