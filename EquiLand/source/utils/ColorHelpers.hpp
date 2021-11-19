#ifndef SOURCE_UTILS_COLORHELPERS_HPP
#define SOURCE_UTILS_COLORHELPERS_HPP

#include <includes_pch.h>

namespace helpers {

	ImVec4 NormalizeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		return ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	// darken_factor in percents
	ImU32 DarkenColor(ImU32 color, int darken_factor = 15)
	{
		auto a = color >> 24;
		auto b = (color << 8) >> 8 + 16;
		auto g = (color << 16) >> 16 + 8;
		auto r = (color << 24) >> 24;

		r -= r / darken_factor;
		g -= g / darken_factor;
		b -= b / darken_factor;
		//a -= a / darken_factor;

		color = IM_COL32(r, g, b, a);
		return color;
	}

	// brighten_factor in percents
	ImU32 BrightenColor(ImU32 color, int brighten_factor = 15)
	{
		auto a = color >> 24;
		auto b = (color << 8) >> 8 + 16;
		auto g = (color << 16) >> 16 + 8;
		auto r = (color << 24) >> 24;

		auto new_r = r + r / brighten_factor;
		auto new_g = g + g / brighten_factor;
		auto new_b = b + b / brighten_factor;
		//auto new_a = a + a / brighten_factor;
		if (new_r > 255)
			new_r = 255;
		if (new_g > 255)
			new_g = 255;
		if (new_b > 255)
			new_b = 255;

		color = IM_COL32(new_r, new_g, new_b, a);
		return color;
	}

}

#endif // !SOURCE_UTILS_COLORHELPERS_HPP