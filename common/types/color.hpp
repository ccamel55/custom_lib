#pragma once

#include <cstdint>

namespace lib::common {

	class color {
	public:
		constexpr color() = default;

		//! \a red range 0u to 255u, \a green range 0u to 255u, \a blue range 0u to 255u, \a alpha range 0u to 255u,
		constexpr color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255u)
			: _red(red)
			, _green(green)
			, _blue(blue)
			, _alpha(alpha) {

		}

//		//! \a hue range 0.f to 360.f, \a saturation range 0.f to 1.f, \a brightness range 0.f to 1.f
//		static color from_hsv(float hue, float saturation, float brightness);
//
//		//! \a hue range 0.f to 360.f, \a saturation range 0.f to 1.f, \a brightness range 0.f to 1.f
//		void to_hsv(float& hue, float& saturation, float& brightness);

	private:
		uint8_t _red = 0;
		uint8_t _green = 0;
		uint8_t _blue = 0;
		uint8_t _alpha = 0;
	};
}