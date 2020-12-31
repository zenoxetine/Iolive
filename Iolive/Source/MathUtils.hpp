#pragma once

#include <cmath>

namespace MathUtils {
	constexpr float Lerp(float start, float end, float percent)
	{
		return (start + percent * (end - start));
	}

	constexpr float Normalize(float value, float min, float max){
		return (value - min) / (max - min);
	};

	inline float Abs(float value)
	{
		return std::abs(value);
	}

	inline float Difference(float value1, float value2)
	{
		return std::abs(std::abs(value1) - std::abs(value2));
	}
}