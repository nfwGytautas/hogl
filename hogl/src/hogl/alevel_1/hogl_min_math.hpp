#pragma once

/**
 * @brief This file contains the necessary abstractions around glm, as with everything with hogl core
 * all functions can be called with glm since these are just wrappers around the functions that hogl needs.
*/

#include "hogl/hogl_types.hpp"
#include <array>

HOGL_NSPACE_BEGIN_L1

/**
 * @brief Vector of 2 T components
 * @tparam T Component type
*/
template<typename T>
class hogl_v2
{
public:
	/**
	 * @brief Create an empty float vector
	*/
	hogl_v2()
		: m_values{ 0, 0 }
	{}

	/**
	 * @brief Create an initialized float vector
	 * @param x X component value of the vector
	 * @param y Y component	value of the vector
	*/
	hogl_v2(T x, T y)
		: m_values{ x, y }
	{}

	/**
	 * @brief Returns the values of the vector as a constant array
	 * @return const pointer to array of size 2
	*/
	inline const T* ptr_const() const
	{
		return m_values.data();
	}

	/**
	 * @brief Returns the values of the vector as a mutable array
	 * @return Pointer to array of size 2
	*/
	inline T* ptr_mut()
	{
		return m_values.data();
	}

	/**
	 * @brief Add operator
	 * @param v Vector to add
	*/
	hogl_v2<T> operator+(const hogl_v2<T>& v) {
		return hogl_v2<T>(
			m_values[0] + v.m_values[0],
			m_values[1] + v.m_values[1],
		);
	}
private:
	std::array<T, 2> m_values;
};

/**
 * @brief Vector of 3 T components
 * @tparam T Component type
*/
template<typename T>
class hogl_v3
{
public:
	/**
	 * @brief Create an empty float vector
	*/
	hogl_v3()
		: m_values{0, 0, 0}
	{}

	/**
	 * @brief Create an initialized float vector
	 * @param x X component value of the vector
	 * @param y Y component	value of the vector
	 * @param z Z component value of the vector
	*/
	hogl_v3(T x, T y, T z)
		: m_values{x, y, z}
	{}

	/**
	 * @brief Returns the values of the vector as a constant array
	 * @return const pointer to array of size 3
	*/
	inline const T* ptr_const() const
	{
		return m_values.data();
	}

	/**
	 * @brief Returns the values of the vector as a mutable array
	 * @return Pointer to array of size 3
	*/
	inline T* ptr_mut()
	{
		return m_values.data();
	}

	/**
	 * @brief Add operator
	 * @param v Vector to add
	*/
	hogl_v3<T> operator+(const hogl_v3<T>& v) {
		return hogl_v3<T>(
			m_values[0] + v.m_values[0],
			m_values[1] + v.m_values[1],
			m_values[2] + v.m_values[2]
		);
	}
private:
	std::array<T, 3> m_values;
};

/**
 * @brief Vector of 4 T components
 * @tparam T Component type
*/
template<typename T>
class hogl_v4
{
public:
	/**
	 * @brief Create an empty float vector
	*/
	hogl_v4()
		: m_values{ 0, 0, 0, 0 }
	{}

	/**
	 * @brief Create an initialized float vector
	 * @param x X component value of the vector
	 * @param y Y component	value of the vector
	 * @param z Z component value of the vector
	 * @param w W component value of the vector
	*/
	hogl_v4(T x, T y, T z, T w)
		: m_values{ x, y, z, w }
	{}

	/**
	 * @brief Returns the values of the vector as a constant array
	 * @return const pointer to array of size 4
	*/
	inline const T* ptr_const() const
	{
		return m_values.data();
	}

	/**
	 * @brief Returns the values of the vector as a mutable array
	 * @return Pointer to array of size 4
	*/
	inline T* ptr_mut()
	{
		return m_values.data();
	}

	/**
	 * @brief Add operator
	 * @param v Vector to add
	*/
	hogl_v4<T> operator+(const hogl_v4<T>& v) {
		return hogl_v3<T>(
			m_values[0] + v.m_values[0],
			m_values[1] + v.m_values[1],
			m_values[2] + v.m_values[2],
			m_values[3] + v.m_values[3]
			);
	}
private:
	std::array<T, 4> m_values;
};

/**
 * @brief Matrix of 4x4 T components
 * @tparam T Component type
*/
template<typename T>
class hogl_m44
{
public:
	/**
	 * @brief Initializes an empty 4x4 matrix
	*/
	hogl_m44()
	{}

	/**
	 * @brief Creates a 4x4 matrix from a float pointer
	 * @param values 
	*/
	hogl_m44(T* values)
	{
		// We copy only the 16 first values just in case the array is larger and it would overflow
		std::copy(values, values + (4 * 4), m_values.data());
	}

	/**
	 * @brief Returns the values of the vector as a constant array
	 * @return const pointer to array of size 4x4
	*/
	inline const T* ptr_const() const
	{
		return m_values.data();
	}

	/**
	 * @brief Returns the values of the vector as a mutable array
	 * @return Pointer to array of size 4x4
	*/
	inline T* ptr_mut()
	{
		return m_values.data();
	}
private:
	std::array<T, 4 * 4> m_values = {};
};

/**
 * @brief Generates an empty hogl_m44 identity matrix
 * @return Identity matrix
*/
HOGL_API hogl_m44<float> hogl_m44_empty();

/**
 * @brief Generate a view matrix that transforms coordinates in such a way that it the eye looks at the target 
 * @param eye Position of the eye or camera
 * @param target Position of the target or object we are looking at
 * @param up Vector showing the direction where the up direction is
 * @return View matrix
*/
HOGL_API hogl_m44<float> hogl_look_at(const hogl_v3<float>& eye, const hogl_v3<float>& target, const hogl_v3<float>& up);

/**
 * @brief Creates a perspective matrix
 * @param fov Field of vision for the camera using this matrix in degrees
 * @param aspectRatio Aspect ratio of the view
 * @param near Near clipping plain value
 * @param far Far clipping plain value
 * @return Perspective matrix
*/
HOGL_API hogl_m44<float> hogl_perspective(const float& fov, const float& aspectRatio, const float& near, const float& far);

/**
 * @brief Translate the specified matrix by a value vector
 * @param matrix Matrix to translate
 * @param value Value to translate by
 * @return Translated matrix
*/
HOGL_API hogl_m44<float> hogl_translate(const hogl_m44<float>& matrix, const hogl_v3<float> value);

/**
 * @brief Scale the specified matrix by a value vector
 * @param matrix Matrix to translate
 * @param value Value to scale by
 * @return Scaled matrix
*/
HOGL_API hogl_m44<float> hogl_scale(const hogl_m44<float>& matrix, const hogl_v3<float> value);

/**
 * @brief Pad a 3 component vector to 4 component vector
 * @tparam T Type of the vector
 * @param vec Vector to pad
 * @return Vector of 4 components
*/
template<typename T>
hogl_v4<T> hogl_pad_v3(const hogl_v3<T>& vec)
{
	return hogl_v4<T>(vec.ptr_const()[0], vec.ptr_const()[1], vec.ptr_const()[2], {});
}

HOGL_NSPACE_END_L1
