#pragma once

#include <cmath>
#include <cstdlib>

namespace lib::common
{
	class vector3D
	{
	public:
		constexpr vector3D() = default;

		constexpr vector3D(float x, float y, float z) :
			_x(x), _y(y), _z(z)
		{
		}

		vector3D operator+(const vector3D& in) const
		{
			return {
				_x + in._x,
				_y + in._y,
				_z + in._z
			};
		}

		constexpr vector3D& operator+=(const vector3D& in)
		{
			this->_x += in._x;
			this->_y += in._y;
			this->_z += in._z;

			return *this;
		}

		vector3D operator+(float in) const
		{
			return {
				_x + in,
				_y + in,
				_z + in
			};
		}

		constexpr vector3D& operator+=(float in)
		{
			this->_x += in;
			this->_y += in;
			this->_z += in;

			return *this;
		}

		vector3D operator-(const vector3D& in) const
		{
			return {
				_x - in._x,
				_y - in._y,
				_z - in._z
			};
		}

		constexpr vector3D& operator-=(const vector3D& in)
		{
			this->_x -= in._x;
			this->_y -= in._y;
			this->_z -= in._z;

			return *this;
		}

		vector3D operator-(float in) const
		{
			return {
				_x - in,
				_y - in,
				_z - in
			};
		}

		constexpr vector3D& operator-=(float in)
		{
			this->_x -= in;
			this->_y -= in;
			this->_z -= in;

			return *this;
		}

		vector3D operator*(const vector3D& in) const
		{
			return {
				_x * in._x,
				_y * in._y,
				_z * in._z
			};
		}

		constexpr vector3D& operator*=(const vector3D& in)
		{
			this->_x *= in._x;
			this->_y *= in._y;
			this->_z *= in._z;

			return *this;
		}

		vector3D operator*(float in) const
		{
			return {
				_x * in,
				_y * in,
				_z * in
			};
		}

		constexpr vector3D& operator*=(float in)
		{
			this->_x *= in;
			this->_y *= in;
			this->_z *= in;

			return *this;
		}

		vector3D operator/(const vector3D& in) const
		{
			return {
				_x / in._x,
				_y / in._y,
				_z / in._z
			};
		}

		constexpr vector3D& operator/=(const vector3D& in)
		{
			this->_x /= in._x;
			this->_y /= in._y;
			this->_z /= in._z;

			return *this;
		}

		vector3D operator/(float in) const
		{
			return {
				_x / in,
				_y / in,
				_z / in
			};
		}

		constexpr vector3D& operator/=(float in)
		{
			this->_x /= in;
			this->_y /= in;
			this->_z /= in;

			return *this;
		}

		bool operator==(const vector3D& vec_base) const
		{
			return this->is_equal(vec_base);
		}

		bool operator!=(const vector3D& vec_base) const
		{
			return !this->is_equal(vec_base);
		}

		constexpr vector3D& operator=(const vector3D& vec_base)
		{
			this->_x = vec_base._x;
			this->_y = vec_base._y;
			this->_z = vec_base._z;
			return *this;
		}

		constexpr vector3D& operator=(const vector2D& vec_base2D)
		{
			this->_x = vec_base2D._x;
			this->_y = vec_base2D._y;
			this->_z = 0.0f;
			return *this;
		}

		[[nodiscard]] bool is_equal(const vector3D& vec_equal, const float error_margin = std::numeric_limits<float>::epsilon()) const
		{
			return fabsf(this->_x - vec_equal._x) < error_margin && fabsf(this->_y - vec_equal._y) < error_margin && fabsf(this->_z - vec_equal._z) < error_margin;
		}

		[[nodiscard]] float length() const
		{
			return std::sqrt(_x * _x + _y * _y + _z * _z);
		}

		[[nodiscard]] float length2D() const
		{
			return std::sqrt(_x * _x + _y * _y);
		}

		[[nodiscard]] float length_sqr() const
		{
			return (_x * _x + _y * _y + _z * _z);
		}

		[[nodiscard]] float length2D_sqr() const
		{
			return (_x * _x + _y * _y);
		}

		[[nodiscard]] vector3D normalize() const
		{
			auto out = *this;
			const auto length = out.length();

			if (length != 0.f)
			{
				out = out / length;
			}
			else
			{
				out._x = out._y = out._z = 0;
			}

			return out;
		}

		[[nodiscard]] float dist(const vector3D& in) const
		{
			return vector3D(_x - in._x, _y - in._y, _z - in._z).length();
		}

		[[nodiscard]] float dot(const vector3D& in) const
		{
			return (_x * in._x + _y * in._y + _z * in._z);
		}

	public:
		float _x = 0.f;
		float _y = 0.f;
		float _z = 0.f;
	};
}