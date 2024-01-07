#pragma once

namespace lib
{
template <class t> class _point4D
{
public:
	constexpr _point4D() = default;
	constexpr _point4D& operator=(const _point4D& in) = default;

	constexpr _point4D(auto x, auto y, auto z, auto w) :
		x(static_cast<t>(x)), y(static_cast<t>(y)), z(static_cast<t>(z)), w(static_cast<t>(w))
	{
	}

	_point4D operator+(const _point4D& in) const
	{
		return {x + in.x, y + in.y, z + in.z, w + in.w};
	}

	_point4D operator-(const _point4D& in) const
	{
		return {x - in.x, y - in.y, z - in.z, w - in.w};
	}

	bool operator==(const _point4D& in) const
	{
		return x == in.x && y == in.y && z == in.z && w == in.w;
	}

	bool operator!=(const _point4D& in) const
	{
		return x != in.x || y != in.y || z != in.z || w != in.w;
	}

public:
	t x = static_cast<t>(0);
	t y = static_cast<t>(0);
	t z = static_cast<t>(0);
	t w = static_cast<t>(0);
};

using point4Df = _point4D<float>;
using point4Di = _point4D<int>;
using point4Dd = _point4D<double>;
using point4Dl = _point4D<long>;
}  // namespace lib