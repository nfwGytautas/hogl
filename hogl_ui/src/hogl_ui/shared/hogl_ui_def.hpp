#pragma once

#ifdef __cplusplus
	#ifdef HOGL_UI_EXPORT
		#define HOGL_UI_API __declspec(dllexport)
	#else
		#define HOGL_UI_API __declspec(dllimport)
	#endif
#else
	#define HOGL_UI_API
#endif
