#pragma once

namespace lib::common {

	class color {
	public:
		constexpr color() = default;

        constexpr color(int r, int g, int b, int a = 255)
			: _r(r) , _g(g) , _b(b) , _a(a) {
		}
	public:
		int _r = 0;
		int _g = 0;
		int _b = 0;
		int _a = 255;
	};
}