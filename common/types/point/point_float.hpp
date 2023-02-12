#pragma once

namespace lib::common {

	class point_float {
	public:
		constexpr point_float() = default;

		constexpr point_float(float x, float y)
			: _x(x)
			, _y(y) {

		}

	private:
		float _x = 0.f;
		float _y = 0.f;
	};
}