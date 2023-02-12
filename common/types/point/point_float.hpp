#pragma once

namespace lib::common {

	class point_float {
	public:
	  	constexpr point_float() = default;
		constexpr point_float(float x, float y)
			: _x(x)
            , _y(y) {
		}

		point_float operator+(const point_float& in) const {
			return {
				_x + in._x,
				_y + in._y
			};
		}

		point_float operator-(const point_float& in) const {
			return {
				_x - in._x,
				_y - in._y
			};
		}

		constexpr point_float& operator=(const point_float& in) {
			this->_x = in._x; this->_y = in._y;
			return *this;
		}
	  public:
		float _x = 0.f;
		float _y = 0.f;
	};
}