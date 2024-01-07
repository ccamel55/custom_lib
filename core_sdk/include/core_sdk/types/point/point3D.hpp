#pragma once

namespace lib
{
template <class t> class _point3D
{
public:
	constexpr _point3D() = default;

	constexpr _point3D(auto x, auto y, auto z) : x(static_cast<t>(x)), y(static_cast<t>(y)), z(static_cast<t>(z))
	{
	}

	_point3D operator+(const _point3D& in) const
	{
		return {x + in.x, y + in.y, z + in.z};
	}

	constexpr _point3D& operator+=(const _point3D& in)
	{
		this->x += in.x;
		this->y += in.y;
		this->z += in.z;

		return *this;
	}

	_point3D operator+(float in) const
	{
		return {x + in, y + in, z + in};
	}

	constexpr _point3D& operator+=(float in)
	{
		this->x += in;
		this->y += in;
		this->z += in;

		return *this;
	}

	_point3D operator-(const _point3D& in) const
	{
		return {x - in.x, y - in.y, z - in.z};
	}

	constexpr _point3D& operator-=(const _point3D& in)
	{
		this->x -= in.x;
		this->y -= in.y;
		this->z -= in.z;

		return *this;
	}

	_point3D operator-(float in) const
	{
		return {x - in, y - in, z - in};
	}

	constexpr _point3D& operator-=(float in)
	{
		this->x -= in;
		this->y -= in;
		this->z -= in;

		return *this;
	}

	_point3D operator*(const _point3D& in) const
	{
		return {x * in.x, y * in.y, z * in.z};
	}

	constexpr _point3D& operator*=(const _point3D& in)
	{
		this->x *= in.x;
		this->y *= in.y;
		this->z *= in.z;

		return *this;
	}

	_point3D operator*(float in) const
	{
		return {x * in, y * in, z * in};
	}

	constexpr _point3D& operator*=(float in)
	{
		this->x *= in;
		this->y *= in;
		this->z *= in;

		return *this;
	}

	_point3D operator/(const _point3D& in) const
	{
		return {x / in.x, y / in.y, z / in.z};
	}

	constexpr _point3D& operator/=(const _point3D& in)
	{
		this->x /= in.x;
		this->y /= in.y;
		this->z /= in.z;

		return *this;
	}

	_point3D operator/(float in) const
	{
		return {x / in, y / in, z / in};
	}

	constexpr _point3D& operator/=(float in)
	{
		this->x /= in;
		this->y /= in;
		this->z /= in;

		return *this;
	}

    bool operator==(const _point3D& in) const
    {
        return x == in.x && y == in.y && z == in.z;
    }

    bool operator!=(const _point3D& in) const
    {
        return x != in.x || y != in.y || z != in.z;
    }

	constexpr _point3D& operator=(const _point3D& vec_base)
	{
		this->x = vec_base.x;
		this->y = vec_base.y;
		this->z = vec_base.z;
		return *this;
	}

public:
	t x = static_cast<t>(0);
	t y = static_cast<t>(0);
	t z = static_cast<t>(0);
};

using point3Df = _point3D<float>;
using point3Di = _point3D<int>;
using point3Dd = _point3D<double>;
using point3Dl = _point3D<long>;
}  // namespace lib