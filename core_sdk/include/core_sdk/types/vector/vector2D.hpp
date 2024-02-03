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
		const _point2D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return (fabsf(this->x - vec_equal.x) < error_margin && fabsf(this->y - vec_equal.y) < error_margin);
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] float length_sqr() const
	{
		return (x * x + y * y);
	}

	[[nodiscard]] vector2D normalised() const
	{
		// normalise vector so that magnitude is 1
		const auto x_x_y_y = (this->x * this->x) + (this->y * this->y);

		if (x_x_y_y <= 0.f)
		{
			return *this;
		}

		const auto inverse_sqrt = (1.f / std::sqrtf(x_x_y_y));

		return {this->x * inverse_sqrt, this->y * inverse_sqrt};
	}
};
}  // namespace lib