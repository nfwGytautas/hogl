#pragma once

#include <string>
#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"

#define HOGL_OBJECT(type_id) public: const size_t TYPE_ID = type_id; virtual size_t type() const override { return TYPE_ID; } private:

namespace hogl {

	/**
	 * Standard hogl object TYPE_ID values
	 * Shader		0
	 * Mesh			1
	 * Material		2
	 * Texture		3
	 * GPU_buffer	4
	 * Cubemap		5
	 * Framebuffer	6
	 * Renderbuffer	7
	 * UBO			8
	 */

	/**
	 * @brief Base class for almost all C++ hogl objects used to be stored in
	 * the object storage. Not to be mixed up with application related objects.
	 * All class that expand this class must provide implementation for all pure virtual
	 * methods and have a HOGL_TYPE_ID(x); where x is the unique number of the type.
	 */
	class HOGL_CPP_API object {
	public:
		virtual ~object() {};

		/**
		 * @brief Name of the object used mostly for UI and object storage
		 * @return String representation of the object
		*/
		std::string name() const;

		/**
		 * @brief Object type used mostly for UI and object storage
		 * @return size_t type representation number
		*/
		virtual size_t type() const = 0;
	protected:
		std::string m_name = "unnamed";
	};

}
