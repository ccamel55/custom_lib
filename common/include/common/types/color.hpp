#pragma once

namespace lib::common
{
class color_f
{
public:
	constexpr color_f() = default;

	constexpr color_f(float r, float g, float b, float a = 255) : _r(r), _g(g), _b(b), _a(a)
	{
	}

	constexpr color_f(int r, int g, int b, int a = 255) :
		_r(static_cast<float>(r) / 255.f), _g(static_cast<float>(g) / 255.f), _b(static_cast<float>(b) / 255.f),
		_a(static_cast<float>(a) / 255.f)
	{
	}

public:
	float _r = 0.f;
	float _g = 0.f;
	float _b = 0.f;
	float _a = 1.f;
};

class color
{
public:
	constexpr color() = default;

	constexpr color(int r, int g, int b, int a = 255) : _r(r), _g(g), _b(b), _a(a)
	{
	}

	[[nodiscard]] constexpr color_f to_color_f() const
	{
		return {_r, _g, _b, _a};
	}

public:
	int _r = 0;
	int _g = 0;
	int _b = 0;
	int _a = 255;
};
}  // namespace lib::common