#pragma once

#include <cmath>
#include <limits>

#include <core_sdk/types/point/point2D.hpp>

namespace lib
{
class vector2D : public point2Df
{
public:
	constexpr vector2D(float x, float y)
	{
		this->_x = x;
		this->_y = y;
	}

	constexpr vector2D(const point2Df& point)
	{
		this->_x = point._x;
		this->_y = point._y;
	}

	[[nodiscard]] bool is_equal(
		const _point2D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
	{
		return (fabsf(this->_x - vec_equal._x) < error_margin && fabsf(this->_y - vec_equal._y) < error_margin);
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(_x * _x + _y * _y);
	}

	[[nodiscard]] float length_sqr() const
	{
		return (_x * _x + _y * _y);
	}

	[[nodiscard]] vector2D normalised() const
	{
		// normalise vector so that magnitude is 1
		const auto x_x_y_y = (this->_x * this->_x) + (this->_y * this->_y);

		if (x_x_y_y <= 0.f)
		{
			return *this;
		}

		const auto inverse_sqrt = (1.f / std::sqrtf(x_x_y_y));

		return {this->_x * inverse_sqrt, this->_y * inverse_sqrt};
	}
};
}  // namespace lib