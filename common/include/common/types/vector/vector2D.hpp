#pragma once

#include <cmath>
#include <cstdlib>

namespace lib::common
{
class vector2D
{
public:
	constexpr vector2D() = default;

	constexpr vector2D(auto x, auto y) : _x(static_cast<float>(x)), _y(static_cast<float>(y))
	{
	}

	vector2D operator+(const vector2D& in) const
	{
		return {_x + in._x, _y + in._y};
	}

	constexpr vector2D& operator+=(const vector2D& in)
	{
		this->_x += in._x;
		this->_y += in._y;

		return *this;
	}

	vector2D operator+(float in) const
	{
		return {_x + in, _y + in};
	}

	constexpr vector2D& operator+=(float in)
	{
		this->_x += in;
		this->_y += in;

		return *this;
	}

	vector2D operator-(const vector2D& in) const
	{
		return {_x - in._x, _y - in._y};
	}

	constexpr vector2D& operator-=(const vector2D& in)
	{
		this->_x -= in._x;
		this->_y -= in._y;

		return *this;
	}

	vector2D operator-(float in) const
	{
		return {_x - in, _y - in};
	}

	constexpr vector2D& operator-=(float in)
	{
		this->_x -= in;
		this->_y -= in;

		return *this;
	}

	vector2D operator*(const vector2D& in) const
	{
		return {_x * in._x, _y * in._y};
	}

	constexpr vector2D& operator*=(const vector2D& in)
	{
		this->_x *= in._x;
		this->_y *= in._y;

		return *this;
	}

	vector2D operator*(float in) const
	{
		return {_x * in, _y * in};
	}

	constexpr vector2D& operator*=(float in)
	{
		this->_x *= in;
		this->_y *= in;

		return *this;
	}

	vector2D operator/(const vector2D& in) const
	{
		return {_x / in._x, _y / in._y};
	}

	constexpr vector2D& operator/=(const vector2D& in)
	{
		this->_x /= in._x;
		this->_y /= in._y;

		return *this;
	}

	vector2D operator/(float in) const
	{
		return {_x / in, _y / in};
	}

	constexpr vector2D& operator/=(float in)
	{
		this->_x /= in;
		this->_y /= in;

		return *this;
	}

	bool operator==(const vector2D& vec_base) const
	{
		return this->is_equal(vec_base);
	}

	bool operator!=(const vector2D& vec_base) const
	{
		return !this->is_equal(vec_base);
	}

	[[nodiscard]] bool is_equal(
		const vector2D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
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

	[[nodiscard]] common::vector2D normalised() const
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

public:
	float _x = 0.f;
	float _y = 0.f;
};
}  // namespace lib::common