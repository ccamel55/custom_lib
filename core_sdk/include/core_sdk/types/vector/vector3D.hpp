#pragma once

#include <cmath>
#include <limits>

#include <core_sdk/types/point/point3D.hpp>

namespace lib
{
class vector3D : public point3Df
{
public:
	constexpr vector3D() = default;
	constexpr vector3D(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// explicitly not declared explicit (we want implicit conversion)
	constexpr vector3D(const point3Df& point)
	{
		this->x = point.x;
		this->y = point.y;
		this->z = point.z;
	}

	[[nodiscard]] bool is_equal(
		const vector3D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return fabsf(this->x - vec_equal.x) < error_margin && fabsf(this->y - vec_equal.y) < error_margin &&
			   fabsf(this->z - vec_equal.z) < error_margin;
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	[[nodiscard]] float length2D() const
	{
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] float length_sqr() const
	{
		return (x * x + y * y + z * z);
	}

	[[nodiscard]] float length2D_sqr() const
	{
		return (x * x + y * y);
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
			out.x = out.y = out.z = 0;
		}

		return out;
	}

	[[nodiscard]] float dist(const vector3D& in) const
	{
		return vector3D(x - in.x, y - in.y, z - in.z).length();
	}

	[[nodiscard]] float dot(const vector3D& in) const
	{
		return (x * in.x + y * in.y + z * in.z);
	}
};
}  // namespace lib