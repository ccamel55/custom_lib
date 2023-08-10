#pragma once

#include <common/types/color.hpp>

namespace lib::backend::render
{
	struct vertex_2d_t
	{
		vertex_2d_t() :
			_x(0.f), _y(0.f), _color({}), _tx(0.f), _ty(0.f)
		{
		}

		vertex_2d_t(float x, float y, const common::color_f& col, float tx = 0.f, float ty = 0.f) :
			_x(x), _y(y), _color(col), _tx(tx), _ty(ty)
		{
		}

		float _x;
		float _y;

		common::color_f _color;

		float _tx;
		float _ty;
	};
}