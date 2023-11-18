#pragma once

namespace lib
{
template <class t> class _point3D
{
public:
	constexpr _point3D() = default;

	constexpr _point3D(auto x, auto y, auto z) : _x(static_cast<t>(x)), _y(static_cast<t>(y)), _z(static_cast<t>(z))
	{
	}

	_point3D operator+(const _point3D& in) const
	{
		return {_x + in._x, _y + in._y, _z + in._z};
	}

	constexpr _point3D& operator+=(const _point3D& in)
	{
		this->_x += in._x;
		this->_y += in._y;
		this->_z += in._z;

		return *this;
	}

	_point3D operator+(float in) const
	{
		return {_x + in, _y + in, _z + in};
	}

	constexpr _point3D& operator+=(float in)
	{
		this->_x += in;
		this->_y += in;
		this->_z += in;

		return *this;
	}

	_point3D operator-(const _point3D& in) const
	{
		return {_x - in._x, _y - in._y, _z - in._z};
	}

	constexpr _point3D& operator-=(const _point3D& in)
	{
		this->_x -= in._x;
		this->_y -= in._y;
		this->_z -= in._z;

		return *this;
	}

	_point3D operator-(float in) const
	{
		return {_x - in, _y - in, _z - in};
	}

	constexpr _point3D& operator-=(float in)
	{
		this->_x -= in;
		this->_y -= in;
		this->_z -= in;

		return *this;
	}

	_point3D operator*(const _point3D& in) const
	{
		return {_x * in._x, _y * in._y, _z * in._z};
	}

	constexpr _point3D& operator*=(const _point3D& in)
	{
		this->_x *= in._x;
		this->_y *= in._y;
		this->_z *= in._z;

		return *this;
	}

	_point3D operator*(float in) const
	{
		return {_x * in, _y * in, _z * in};
	}

	constexpr _point3D& operator*=(float in)
	{
		this->_x *= in;
		this->_y *= in;
		this->_z *= in;

		return *this;
	}

	_point3D operator/(const _point3D& in) const
	{
		return {_x / in._x, _y / in._y, _z / in._z};
	}

	constexpr _point3D& operator/=(const _point3D& in)
	{
		this->_x /= in._x;
		this->_y /= in._y;
		this->_z /= in._z;

		return *this;
	}

	_point3D operator/(float in) const
	{
		return {_x / in, _y / in, _z / in};
	}

	constexpr _point3D& operator/=(float in)
	{
		this->_x /= in;
		this->_y /= in;
		this->_z /= in;

		return *this;
	}

	bool operator==(const _point3D& vec_base) const
	{
		return this->is_equal(vec_base);
	}

	bool operator!=(const _point3D& vec_base) const
	{
		return !this->is_equal(vec_base);
	}

	constexpr _point3D& operator=(const _point3D& vec_base)
	{
		this->_x = vec_base._x;
		this->_y = vec_base._y;
		this->_z = vec_base._z;
		return *this;
	}

public:
	t _x = static_cast<t>(0);
	t _y = static_cast<t>(0);
	t _z = static_cast<t>(0);
};

using point3Df = _point3D<float>;
using point3Di = _point3D<int>;
using point3Dd = _point3D<double>;
using point3Dl = _point3D<long>;
}  // namespace lib