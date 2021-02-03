#pragma once

#include <unordered_map>
#include <vector>

#include "hogl/shared/hogl_def.hpp"
#include "hogl/shared/pointer.hpp"
#include "hogl/core/object.hpp"
#include "hogl_core/shared/hogl_log.h"

namespace hogl {

	/**
	 * @brief Class used to store all objects, that are created by hogl, be it meshes, material or hogl core primitives
	*/
	class HOGL_CPP_API object_storage {
	public:
		~object_storage();

		/**
		 * @brief Creates a new object with specified parameters
		 * @tparam T Type of the object, must inherit object class
		 * @tparam ...TArgs Argument types
		 * @param ...args Arguments
		 * @return Object reference
		*/
		template<class T, class... TArgs>
		ref<T> create_new(TArgs... args) {
			ref<T> obj = new T(std::forward<TArgs>(args)...);
			m_objects.push_back(obj.as<object>());
			return obj;
		}

		/**
		 * @brief Returns the object with the specified name or invalid reference if not found
		 * @tparam T 
		 * @param name 
		 * @return 
		*/
		template<class T>
		ref<T> reference(const std::string& name) {
			for (const ref<object>& obj : m_objects) {
				if (obj->name() == name) {
					if (obj->type() == T::TYPE_ID) {
						return obj.as<T>();
					}
					else {
						hogl_log_warn("Object found but types don't match returning invalid reference");
						break;
					}
				}
			}

			return nullptr;
		}

	private:
		std::vector<ref<object>> m_objects;
	};

}
