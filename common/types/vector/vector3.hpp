#pragma once
#include <common/types/vector/vector2.hpp>

namespace lib::common {

	class vector3 {
	public:
	  constexpr vector3() = default;
      constexpr vector3(float x, float y, float z)
			: _x(x)
            , _y(y)
            , _z(z) {
		}

	  vector3 operator+(const vector3& in) const {
			return {
				_x + in._x,
				_y + in._y,
				_z + in._z
			};
		}

		constexpr vector3& operator+=(const vector3& in) {

			this->_x += in._x;
			this->_y += in._y;
			this->_z += in._z;

			return *this;
		}

		vector3 operator+(float in) const {
			return {
				_x + in,
				_y + in,
				_z + in
			};
		}

		constexpr vector3& operator+=(float in) {

			this->_x += in;
			this->_y += in;
			this->_z += in;

			return *this;
		}

		vector3 operator-(const vector3& in) const {
			return {
				_x - in._x,
				_y - in._y,
				_z - in._z
			};
		}

		constexpr vector3& operator-=(const vector3& in) {

			this->_x -= in._x;
			this->_y -= in._y;
			this->_z -= in._z;

			return *this;
		}

		vector3 operator-(float in) const {
			return {
				_x - in,
				_y - in,
				_z - in
			};
		}

		constexpr vector3& operator-=(float in) {

			this->_x -= in;
			this->_y -= in;
			this->_z -= in;

			return *this;
		}

		vector3 operator*(const vector3& in) const {
			return {
				_x * in._x,
				_y * in._y,
				_z * in._z
			};
		}

		constexpr vector3& operator*=(const vector3& in) {

			this->_x *= in._x;
			this->_y *= in._y;
			this->_z *= in._z;

			return *this;
		}

		vector3 operator*(float in) const {
			return {
				_x * in,
				_y * in,
				_z * in
			};
		}

		constexpr vector3& operator*=(float in) {

			this->_x *= in;
			this->_y *= in;
			this->_z *= in;

			return *this;
		}

		vector3 operator/(const vector3& in) const {
			return {
				_x / in._x,
				_y / in._y,
				_z / in._z
			};
		}

		constexpr vector3& operator/=(const vector3& in) {

			this->_x /= in._x;
			this->_y /= in._y;
			this->_z /= in._z;

			return *this;
		}

		vector3 operator/(float in) const {
			return {
				_x / in,
				_y / in,
				_z / in
			};
		}

		constexpr vector3& operator/=(float in) {

			this->_x /= in;
			this->_y /= in;
			this->_z /= in;

			return *this;
		}

		bool operator==(const vector3& vecBase) const {
		  	return this->IsEqual(vecBase);
		}

		bool operator!=(const vector3& vecBase) const {
		  	return !this->IsEqual(vecBase);
		}

		constexpr vector3& operator=(const vector3& vecBase) {
			this->_x = vecBase._x; this->_y = vecBase._y; this->_z = vecBase._z;
			return *this;
		}

		constexpr vector3& operator=(const vector2& vecBase2D) {
			this->_x = vecBase2D._x; this->_y = vecBase2D._y; this->_z = 0.0f;
			return *this;
		}

		[[nodiscard]] bool IsEqual(const vector3& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const {

			return
				(std::fabsf(this->_x - vecEqual._x) < flErrorMargin &&
				 std::fabsf(this->_y - vecEqual._y) < flErrorMargin &&
				 std::fabsf(this->_z - vecEqual._z) < flErrorMargin);
		}

		[[nodiscard]] float length() const {
			return std::sqrt(_x * _x + _y * _y + _z * _z);
		}

		[[nodiscard]] float length2D() const {
			return std::sqrt(_x * _x + _y * _y);
		}

		[[nodiscard]] float lengthSqr() const {
		  	return (_x * _x + _y * _y + _z * _z);
		}

		[[nodiscard]] float length2DSqr() const {
		  	return (_x * _x + _y * _y);
		}

		[[nodiscard]] vector3 normalize() const {

			auto out = *this;
			const auto length = out.length();

			if (length != 0.f) {
			  out = out / length;
			}
			else {
			  out._x = out._y = out._z = 0;
			}

			return out;
		}

		[[nodiscard]] float dist(const vector3& in) const {
		  	return vector3(_x - in._x, _y - in._y, _z - in._z).length();
		}

		[[nodiscard]] float dot(const vector3& in) const {
		  	return (_x * in._x + _y * in._y + _z * in._z);
		}
	public:
		float _x = 0.f;
		float _y = 0.f;
		float _z = 0.f;
	};
}