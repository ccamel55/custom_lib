#pragma once

namespace lib
{
template <class t> class _point4D
{
public:
	constexpr _point4D() = default;
	constexpr _point4D& operator=(const _point4D& in) = default;

	constexpr _point4D(auto x, auto y, auto z, auto w) :
		_x(static_cast<t>(x)), _y(static_cast<t>(y)), _z(static_cast<t>(z)), _w(static_cast<t>(w))
	{
	}

	_point4D operator+(const _point4D& in) const
	{
		return {_x + in._x, _y + in._y, _z + in._z, _w + in._w};
	}

	_point4D operator-(const _point4D& in) const
	{
		return {_x - in._x, _y - in._y, _z - in._z, _w - in._w};
	}

	bool operator==(const _point4D& in) const
	{
		return _x == in._x && _y == in._y && _z == in._z && _w == in._w;
	}

	bool operator!=(const _point4D& in) const
	{
		return _x != in._x || _y != in._y || _z != in._z || _w != in._w;
	}

public:
	t _x = static_cast<t>(0);
	t _y = static_cast<t>(0);
	t _z = static_cast<t>(0);
	t _w = static_cast<t>(0);
};

using point4Df = _point4D<float>;
using point4Di = _point4D<int>;
using point4Dd = _point4D<double>;
using point4Dl = _point4D<long>;
}  // namespace lib