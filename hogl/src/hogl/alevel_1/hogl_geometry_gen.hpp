#pragma once

// Common hogl library types
#include "hogl/hogl_types.hpp"

// Handle
#include "hogl/alevel_1/hogl_object_handle.hpp"

HOGL_NSPACE_BEGIN_L1

/**
 * @brief Generates a simple cube object and returns a handle to it, make sure that hogl is initialized before calling this function
 * @return hogl_obj_handle instance of hogl_mesh object
*/
HOGL_API hogl_obj_handle<hogl_mesh> hogl_geometry_cube();

HOGL_NSPACE_END_L1
