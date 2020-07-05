#pragma once

/**
 * @brief hogl library for creating interactive 2D, 3D interactive programs, all of the functions are modular and can be used without
 * each other, for example the windowing solution can be custom made while still using the input and rendering systems, as long as the
 * hogl_wnd struct requirements are met. That being said mixing same functionality, e.g. using glfwPollEvents and using hogl input system
 * might cause problems and is inadvisable
*/

// Common hogl library types
#include "hogl_types.hpp"

// hogl context
#include "hogl_context.hpp"

// hogl window
#include "hogl_wnd.hpp"

// hogl input system
#include "hogl_input.hpp"

// hogl object system
#include "hogl_object.hpp"

// hogl object builder
#include "hogl_builder.hpp"

// hogl object loading system
#include "hogl_loader.hpp"

// hogl render system
#include "hogl_renderer.hpp"
