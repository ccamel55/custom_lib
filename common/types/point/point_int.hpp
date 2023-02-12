#pragma once

namespace lib::common {

	class point_int {
	public:
		constexpr point_int() = default;

		constexpr point_int(int x, int y)
				: _x(x)
				, _y(y) {

		}

	private:
		int _x = 0;
		int _y = 0;
	};
}