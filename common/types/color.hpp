#pragma once

namespace lib::common {

	class color {
	public:
		color()
		  	: _r(0) , _g(0) , _b(0) , _a(0) {
		}

		color(int r, int g, int b, int a = 255)
			: _r(r) , _g(g) , _b(b) , _a(a) {
		}
	public:
		int _r;
		int _g;
		int _b;
		int _a;
	};
}