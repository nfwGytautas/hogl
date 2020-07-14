#pragma once

/**
 * @brief 
 * hogl library for creating interactive 2D, 3D interactive programs, all of the functions are modular and can be used without
 * each other, for example the windowing solution can be custom made while still using the input and rendering systems, as long as the
 * hogl_wnd struct requirements are met. That being said mixing same functionality, e.g. using glfwPollEvents and using hogl input system
 * might cause problems and is inadvisable.
 *
 * Core hogl functionality is built with the idea that at any time a user can override by using internal objects themselves.
 * To accomplish this all modules are independent of each other, so for example you can use the windowing solution but render
 * using the OpenGL API calls without any problem or create OpenGL objects yourself and then pass them to the renderer to render
 * them for you. In order to share data between modules simple structs of data is used.
 * For example hogl_wnd is a struct where the internal GLFWwindow is stored inside a type erased native_window variable.
 *
 * By default hogl core functionality is included by default, however
 * the library also has further abstractions to make working with it easier.
 * The core functionality allows for working with APIs as close as possible while still providing
 * the functionality in a single object, without external libraries to include.
 * Each abstraction layer or in hogl case abstraction level, can be selectively included using compiler macros.
 * Everything that can be done with these abstractions is possible with the core functionality,
 * because its all built on top of what the core hogl functionality offers.
 *
 * Abstraction levels are unique in the fact that I haven't applied the same restriction for them to work independently.
 * Modularity is still preserved, but some classes might directly depend on each other.
*/

// Common hogl library types
#include "hogl/hogl_types.hpp"

/**
 * Core modules
*/

// hogl context
#include "hogl/core/hogl_context.hpp"

// hogl window
#include "hogl/core/hogl_wnd.hpp"

// hogl input system
#include "hogl/core/hogl_input.hpp"

// hogl object system
#include "hogl/core/hogl_object.hpp"

// hogl object builder
#include "hogl/core/hogl_builder.hpp"

// hogl object loading system
#include "hogl/core/hogl_loader.hpp"

// hogl render system
#include "hogl/core/hogl_renderer.hpp"


/**
 * Level 1 abstraction
 * Included in this level:
 *	 + Object handles that allow for easier lifetime management of objects
*/

#ifdef HOGL_INCLUDE_LEVEL_1

// hogl object lifetime handle
#include "hogl/alevel_1/hogl_object_handle.hpp"

#endif
