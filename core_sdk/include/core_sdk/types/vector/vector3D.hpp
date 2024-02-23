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
		const vector3D& vec_equal,
		const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return fabsf(this->x - vec_equal.x) < error_margin && fabsf(this->y - vec_equal.y) < error_margin &&
			   fabsf(this->z - vec_equal.z) < error_margin;
	}

	[[nodiscard]] float dot(const vector3D& in) const
	{
		return (x * in.x + y * in.y + z * in.z);
	}

	[[nodiscard]] float length_sqr() const
	{
		return dot(*this);
	}

	[[nodiscard]] float length_xy_sqr() const
	{
		return (x * x + y * y);
	}

	[[nodiscard]] float length_xz_sqr() const
	{
		return (x * x + z * z);
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(length_sqr());
	}

	[[nodiscard]] float length_xy() const
	{
		return std::sqrt(length_xy_sqr());
	}

	[[nodiscard]] float length_xz() const
	{
		return std::sqrt(length_xz_sqr());
	}

	[[nodiscard]] vector3D normalised() const
	{
		return *this / length_sqr();
	}
};
}  // namespace lib