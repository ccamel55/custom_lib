#pragma once

namespace lib::common
{
template <class t> class _point2D
{
  public:
	constexpr _point2D() = default;
	constexpr _point2D &operator=(const _point2D &in) = default;

	constexpr _point2D(t x, t y) : _x(x), _y(y)
	{
	}

	_point2D operator+(const _point2D &in) const
	{
		return {_x + in._x, _y + in._y};
	}

	_point2D operator-(const _point2D &in) const
	{
		return {_x - in._x, _y - in._y};
	}

  public:
	t _x = static_cast<t>(0);
	t _y = static_cast<t>(0);
};

using point2Df = _point2D<float>;
using point2Di = _point2D<int>;
using point2Dd = _point2D<double>;
using point2Dl = _point2D<long>;
}  // namespace lib::common