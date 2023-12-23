#pragma once

#include <cmath>
#include <limits>

#include <core_sdk/types/point/point3D.hpp>

namespace lib
{
class vector3D : public point3Df
{
public:
	constexpr vector3D(float x, float y, float z)
	{
		this->_x = x;
		this->_y = y;
		this->_z = z;
	}

	constexpr vector3D(const point3Df& point)
	{
		this->_x = point._x;
		this->_y = point._y;
		this->_z = point._z;
	}

	[[nodiscard]] bool is_equal(
		const vector3D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return fabsf(this->_x - vec_equal._x) < error_margin && fabsf(this->_y - vec_equal._y) < error_margin &&
			   fabsf(this->_z - vec_equal._z) < error_margin;
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(_x * _x + _y * _y + _z * _z);
	}

	[[nodiscard]] float length2D() const
	{
		return std::sqrt(_x * _x + _y * _y);
	}

	[[nodiscard]] float length_sqr() const
	{
		return (_x * _x + _y * _y + _z * _z);
	}

	[[nodiscard]] float length2D_sqr() const
	{
		return (_x * _x + _y * _y);
	}

	[[nodiscard]] vector3D normalize() const
	{
		auto out = *this;
		const auto length = out.length();

		if (length != 0.f)
		{
			out = out / length;
		}
		else
		{
			out._x = out._y = out._z = 0;
		}

		return out;
	}

	[[nodiscard]] float dist(const vector3D& in) const
	{
		return vector3D(_x - in._x, _y - in._y, _z - in._z).length();
	}

	[[nodiscard]] float dot(const vector3D& in) const
	{
		return (_x * in._x + _y * in._y + _z * in._z);
	}

public:
	float _x = 0.f;
	float _y = 0.f;
	float _z = 0.f;
};
}  // namespace lib