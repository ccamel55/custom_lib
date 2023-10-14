#pragma once

namespace lib::common
{
template <class t> class _point3D
{
public:
	constexpr _point3D() = default;
	constexpr _point3D& operator=(const _point3D& in) = default;

	constexpr _point3D(auto x, auto y, auto z)
		: _x(static_cast<t>(x))
		, _y(static_cast<t>(y))
		, _z(static_cast<t>(z))
	{
	}

	_point3D operator+(const _point3D& in) const
	{
		return {_x + in._x, _y + in._y, _z + in._z};
	}

	_point3D operator-(const _point3D& in) const
	{
		return {_x - in._x, _y - in._y, _z - in._z};
	}

	bool operator==(const _point3D& in) const
	{
		return _x == in._x && _y == in._y && _z == in._z;
	}

	bool operator!=(const _point3D& in) const
	{
		return _x != in._x || _y != in._y || _z != in._z;
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
}  // namespace lib::common