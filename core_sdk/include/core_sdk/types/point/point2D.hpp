#pragma once

namespace lib
{
template <class t> class _point2D
{
public:
	constexpr _point2D() = default;
	constexpr _point2D& operator=(const _point2D& in) = default;

	constexpr _point2D(auto x, auto y) : x(static_cast<t>(x)), y(static_cast<t>(y))
	{
	}

	_point2D operator+(const _point2D& in) const
	{
		return {x + in.x, y + in.y};
	}

	constexpr _point2D& operator+=(const _point2D& in)
	{
		this->x += in.x;
		this->y += in.y;

		return *this;
	}

	_point2D operator+(float in) const
	{
		return {x + in, y + in};
	}

	constexpr _point2D& operator+=(float in)
	{
		this->x += in;
		this->y += in;

		return *this;
	}

	_point2D operator-(const _point2D& in) const
	{
		return {x - in.x, y - in.y};
	}

	constexpr _point2D& operator-=(const _point2D& in)
	{
		this->x -= in.x;
		this->y -= in.y;

		return *this;
	}

	_point2D operator-(float in) const
	{
		return {x - in, y - in};
	}

	constexpr _point2D& operator-=(float in)
	{
		this->x -= in;
		this->y -= in;

		return *this;
	}

	_point2D operator*(const _point2D& in) const
	{
		return {x * in.x, y * in.y};
	}

	constexpr _point2D& operator*=(const _point2D& in)
	{
		this->x *= in.x;
		this->y *= in.y;

		return *this;
	}

	_point2D operator*(float in) const
	{
		return {x * in, y * in};
	}

	constexpr _point2D& operator*=(float in)
	{
		this->x *= in;
		this->y *= in;

		return *this;
	}

	_point2D operator/(const _point2D& in) const
	{
		return {x / in.x, y / in.y};
	}

	constexpr _point2D& operator/=(const _point2D& in)
	{
		this->x /= in.x;
		this->y /= in.y;

		return *this;
	}

	_point2D operator/(float in) const
	{
		return {x / in, y / in};
	}

	constexpr _point2D& operator/=(float in)
	{
		this->x /= in;
		this->y /= in;

		return *this;
	}

    bool operator==(const _point2D& in) const
    {
        return x == in.x && y == in.y;
    }

    bool operator!=(const _point2D& in) const
    {
        return x != in.x || y != in.y;
    }

	[[nodiscard]] float dot(const _point2D& in) const
	{
		return (x * in.x + y * in.y);
	}

public:
	t x = static_cast<t>(0);
	t y = static_cast<t>(0);
};

using point2Df = _point2D<float>;
using point2Di = _point2D<int>;
using point2Dd = _point2D<double>;
using point2Dl = _point2D<long>;
}  // namespace lib