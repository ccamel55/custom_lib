#pragma once

#include <cmath>
#include <limits>

namespace lib::common {

	class vector_2D {
	public:
	  	constexpr vector_2D()
		  	: _x(0.f), _y(0.f) {
		}

		constexpr vector_2D(float x, float y)
			: _x(x), _y(y) {
		}

		vector_2D operator+(const vector_2D& in) const {
			return {
				_x + in._x,
				_y + in._y
			};
		}

		constexpr vector_2D& operator+=(const vector_2D& in) {

			this->_x += in._x;
			this->_y += in._y;

			return *this;
		}

		vector_2D operator+(float in) const {
			return {
				_x + in,
				_y + in
			};
		}

		constexpr vector_2D& operator+=(float in) {

			this->_x += in;
			this->_y += in;

			return *this;
		}

		vector_2D operator-(const vector_2D& in) const {
			return {
				_x - in._x,
				_y - in._y
			};
		}

		constexpr vector_2D& operator-=(const vector_2D& in) {

			this->_x -= in._x;
			this->_y -= in._y;

			return *this;
		}

		vector_2D operator-(float in) const {
			return {
				_x - in,
				_y - in
			};
		}

		constexpr vector_2D& operator-=(float in) {

			this->_x -= in;
			this->_y -= in;

			return *this;
		}

		vector_2D operator*(const vector_2D& in) const {
			return {
				_x * in._x,
				_y * in._y
			};
		}

		constexpr vector_2D& operator*=(const vector_2D& in) {

			this->_x *= in._x;
			this->_y *= in._y;

			return *this;
		}

		vector_2D operator*(float in) const {
			return {
				_x * in,
				_y * in
			};
		}

		constexpr vector_2D& operator*=(float in) {

			this->_x *= in;
			this->_y *= in;

			return *this;
		}

		vector_2D operator/(const vector_2D& in) const {
			return {
				_x / in._x,
				_y / in._y
			};
		}

		constexpr vector_2D& operator/=(const vector_2D& in) {

			this->_x /= in._x;
			this->_y /= in._y;

			return *this;
		}

		vector_2D operator/(float in) const {
			return {
				_x / in,
				_y / in
			};
		}

		constexpr vector_2D& operator/=(float in) {

			this->_x /= in;
			this->_y /= in;

			return *this;
		}

		bool operator==(const vector_2D& vecBase) const {
		  	return this->IsEqual(vecBase);
		}

		bool operator!=(const vector_2D& vecBase) const {
		  	return !this->IsEqual(vecBase);
		}

		[[nodiscard]] bool IsEqual(const vector_2D& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const {

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
		float _x{};
		float _y{};
	};
}