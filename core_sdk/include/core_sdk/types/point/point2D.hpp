#pragma once

namespace lib
{
template <class t> class _point2D
{
public:
	constexpr _point2D() = default;
	constexpr _point2D& operator=(const _point2D& in) = default;

	constexpr _point2D(auto x, auto y) : _x(static_cast<t>(x)), _y(static_cast<t>(y))
	{
	}

	_point2D operator+(const _point2D& in) const
	{
		return {_x + in._x, _y + in._y};
	}

	constexpr _point2D& operator+=(const _point2D& in)
	{
		this->_x += in._x;
		this->_y += in._y;

		return *this;
	}

	_point2D operator+(float in) const
	{
		return {_x + in, _y + in};
	}

	constexpr _point2D& operator+=(float in)
	{
		this->_x += in;
		this->_y += in;

		return *this;
	}

	_point2D operator-(const _point2D& in) const
	{
		return {_x - in._x, _y - in._y};
	}

	constexpr _point2D& operator-=(const _point2D& in)
	{
		this->_x -= in._x;
		this->_y -= in._y;

		return *this;
	}

	_point2D operator-(float in) const
	{
		return {_x - in, _y - in};
	}

	constexpr _point2D& operator-=(float in)
	{
		this->_x -= in;
		this->_y -= in;

		return *this;
	}

	_point2D operator*(const _point2D& in) const
	{
		return {_x * in._x, _y * in._y};
	}

	constexpr _point2D& operator*=(const _point2D& in)
	{
		this->_x *= in._x;
		this->_y *= in._y;

		return *this;
	}

	_point2D operator*(float in) const
	{
		return {_x * in, _y * in};
	}

	constexpr _point2D& operator*=(float in)
	{
		this->_x *= in;
		this->_y *= in;

		return *this;
	}

	_point2D operator/(const _point2D& in) const
	{
		return {_x / in._x, _y / in._y};
	}

	constexpr _point2D& operator/=(const _point2D& in)
	{
		this->_x /= in._x;
		this->_y /= in._y;

		return *this;
	}

	_point2D operator/(float in) const
	{
		return {_x / in, _y / in};
	}

	constexpr _point2D& operator/=(float in)
	{
		this->_x /= in;
		this->_y /= in;

		return *this;
	}

	bool operator==(const _point2D& vec_base) const
	{
		return this->is_equal(vec_base);
	}

	bool operator!=(const _point2D& vec_base) const
	{
		return !this->is_equal(vec_base);
	}

public:
	t _x = static_cast<t>(0);
	t _y = static_cast<t>(0);
};

using point2Df = _point2D<float>;
using point2Di = _point2D<int>;
using point2Dd = _point2D<double>;
using point2Dl = _point2D<long>;
}  // namespace lib