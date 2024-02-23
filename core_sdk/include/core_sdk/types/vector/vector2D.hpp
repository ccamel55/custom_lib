#pragma once

#include <cmath>
#include <limits>

#include <core_sdk/types/point/point2D.hpp>

namespace lib
{
class vector2D : public point2Df
{
public:
	constexpr vector2D() = default;
	constexpr vector2D(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	// explicitly not declared explicit (we want implicit conversion)
	constexpr vector2D(const point2Df& point)
	{
		this->x = point.x;
		this->y = point.y;
	}

	[[nodiscard]] bool is_equal(
		const _point2D& vec_equal,
		const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return (fabsf(this->x - vec_equal.x) < error_margin && fabsf(this->y - vec_equal.y) < error_margin);
	}

	[[nodiscard]] float length_sqr() const
	{
		return dot(*this);
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(length_sqr());
	}

	[[nodiscard]] vector2D normalised() const
	{
		return *this / length_sqr();
	}
};
}  // namespace lib