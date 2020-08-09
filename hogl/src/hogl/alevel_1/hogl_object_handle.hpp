#pragma once

// Common hogl library types
#include "hogl/hogl_types.hpp"

// Objects that are wrapped
#include "hogl/core/hogl_object.hpp"

// Free function
#include "hogl/core/hogl_builder.hpp"

HOGL_NSPACE_BEGIN_L1

/**
 * @brief hogl object lifetime handle, this is not a reference counted handle if a handle gets destroyed so does the resource
 * @tparam T Object to wrap
*/
template<class T>
class hogl_obj_handle
{
public:
	/**
	 * @brief Create empty handle
	*/
	hogl_obj_handle()
	{}

	/**
	 * @brief Create handle with the object
	 * @param obj Object to handle
	*/
	hogl_obj_handle(T* obj)
		: m_owned(true), m_object(obj)
	{}

	~hogl_obj_handle()
	{
		if (m_owned && m_object)
		{
			// Free the resource here
			hogl_free(m_object);
		}
	}

	/**
	 * Dereference operator used to get the object instance
	 * @return Underlying object
	*/
	T& operator*()
	{
		return *m_object;
	}

	/**
	 * Member access operator
	 * @return Underlying object const pointer
	*/
	T* operator->() const
	{
		return m_object;
	}

	/**
	 * @brief Operator for converting straight to pointer type
	 * @return Pointer to the object
	*/
	operator T*() const 
	{ 
		return m_object; 
	}

	/**
	 * @brief Take ownership of the object
	 * @return Underlying object
	*/
	T* own()
	{
		m_owned = false;
		return m_object;
	}

	/**
	 * @brief Take ownership of the object
	 * @param obj Object to take ownership of
	*/
	void own(T* obj)
	{
		m_owned = true;
		m_object = obj;
	}

	/**
	 * Assignment operator used to assign raw object to this handle, after this call the object is owned
	 * @param object Object to wrap
	 * @return This handle
	 */
	hogl_obj_handle<T>& operator=(T* object)
	{
		m_owned = true;
		m_object = object;
		return *this;
	}
private:
	bool m_owned = true;
	T* m_object = nullptr;
};

HOGL_NSPACE_END_L1
