#pragma once

#include <cmath>
#include <limits>

namespace lib::common {

	class vector2 {
	public:
	  	constexpr vector2() = default;
		constexpr vector2(float x, float y)
			: _x(x)
            , _y(y) {
		}

		vector2 operator+(const vector2& in) const {
			return {
				_x + in._x,
				_y + in._y
			};
		}

		constexpr vector2& operator+=(const vector2& in) {

			this->_x += in._x;
			this->_y += in._y;

			return *this;
		}

		vector2 operator+(float in) const {
			return {
				_x + in,
				_y + in
			};
		}

		constexpr vector2& operator+=(float in) {

			this->_x += in;
			this->_y += in;

			return *this;
		}

		vector2 operator-(const vector2& in) const {
			return {
				_x - in._x,
				_y - in._y
			};
		}

		constexpr vector2& operator-=(const vector2& in) {

			this->_x -= in._x;
			this->_y -= in._y;

			return *this;
		}

		vector2 operator-(float in) const {
			return {
				_x - in,
				_y - in
			};
		}

		constexpr vector2& operator-=(float in) {

			this->_x -= in;
			this->_y -= in;

			return *this;
		}

		vector2 operator*(const vector2& in) const {
			return {
				_x * in._x,
				_y * in._y
			};
		}

		constexpr vector2& operator*=(const vector2& in) {

			this->_x *= in._x;
			this->_y *= in._y;

			return *this;
		}

		vector2 operator*(float in) const {
			return {
				_x * in,
				_y * in
			};
		}

		constexpr vector2& operator*=(float in) {

			this->_x *= in;
			this->_y *= in;

			return *this;
		}

		vector2 operator/(const vector2& in) const {
			return {
				_x / in._x,
				_y / in._y
			};
		}

		constexpr vector2& operator/=(const vector2& in) {

			this->_x /= in._x;
			this->_y /= in._y;

			return *this;
		}

		vector2 operator/(float in) const {
			return {
				_x / in,
				_y / in
			};
		}

		constexpr vector2& operator/=(float in) {

			this->_x /= in;
			this->_y /= in;

			return *this;
		}

		bool operator==(const vector2& vecBase) const {
		  	return this->IsEqual(vecBase);
		}

		bool operator!=(const vector2& vecBase) const {
		  	return !this->IsEqual(vecBase);
		}

		[[nodiscard]] bool IsEqual(const vector2& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const {

			return
				(std::fabsf(this->_x - vecEqual._x) < flErrorMargin &&
				 std::fabsf(this->_y - vecEqual._y) < flErrorMargin);
		}

		[[nodiscard]] float length() const {
			return std::sqrt(_x * _x + _y * _y);
		}

		[[nodiscard]] float lengthSqr() const {
			return (_x * _x + _y * _y);
		}

	public:
		float _x = 0.f;
		float _y = 0.f;
	};
}