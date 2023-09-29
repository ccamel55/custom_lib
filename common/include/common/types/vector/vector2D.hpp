#pragma once

#include <cmath>
#include <cstdlib>

namespace lib::common
{
class vector2D
{
  public:
	constexpr vector2D() = default;

	constexpr vector2D(float x, float y) : _x(x), _y(y)
	{
	}

	vector2D operator+(const vector2D &in) const
	{
		return {_x + in._x, _y + in._y};
	}

	constexpr vector2D &operator+=(const vector2D &in)
	{
		this->_x += in._x;
		this->_y += in._y;

		return *this;
	}

	vector2D operator+(float in) const
	{
		return {_x + in, _y + in};
	}

	constexpr vector2D &operator+=(float in)
	{
		this->_x += in;
		this->_y += in;

		return *this;
	}

	vector2D operator-(const vector2D &in) const
	{
		return {_x - in._x, _y - in._y};
	}

	constexpr vector2D &operator-=(const vector2D &in)
	{
		this->_x -= in._x;
		this->_y -= in._y;

		return *this;
	}

	vector2D operator-(float in) const
	{
		return {_x - in, _y - in};
	}

	constexpr vector2D &operator-=(float in)
	{
		this->_x -= in;
		this->_y -= in;

		return *this;
	}

	vector2D operator*(const vector2D &in) const
	{
		return {_x * in._x, _y * in._y};
	}

	constexpr vector2D &operator*=(const vector2D &in)
	{
		this->_x *= in._x;
		this->_y *= in._y;

		return *this;
	}

	vector2D operator*(float in) const
	{
		return {_x * in, _y * in};
	}

	constexpr vector2D &operator*=(float in)
	{
		this->_x *= in;
		this->_y *= in;

		return *this;
	}

	vector2D operator/(const vector2D &in) const
	{
		return {_x / in._x, _y / in._y};
	}

	constexpr vector2D &operator/=(const vector2D &in)
	{
		this->_x /= in._x;
		this->_y /= in._y;

		return *this;
	}

	vector2D operator/(float in) const
	{
		return {_x / in, _y / in};
	}

	constexpr vector2D &operator/=(float in)
	{
		this->_x /= in;
		this->_y /= in;

		return *this;
	}

	bool operator==(const vector2D &vec_base) const
	{
		return this->is_equal(vec_base);
	}

	bool operator!=(const vector2D &vec_base) const
	{
		return !this->is_equal(vec_base);
	}

	[[nodiscard]] bool is_equal(
		const vector2D &vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
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

  public:
	float _x = 0.f;
	float _y = 0.f;
};
}  // namespace lib::common