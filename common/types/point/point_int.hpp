#pragma once

namespace lib::common {

	class point_int {
	public:
		  constexpr point_int()
			  : _x(0), _y(0) {
		  }

		  constexpr point_int(int x, int y)
			  : _x(x), _y(y) {
		  }

		  point_int operator+(const point_int& in) const {
			  return {
				  _x + in._x,
				  _y + in._y
			  };
		  }

		  point_int operator-(const point_int& in) const {
			  return {
				  _x - in._x,
				  _y - in._y
			  };
		  }

		  constexpr point_int& operator=(const point_int& in) {
			  this->_x = in._x; this->_y = in._y;
			  return *this;
		  }
	public:
		int _x{};
		int _y{};
	};
}