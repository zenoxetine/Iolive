#pragma once
#include <cmath>

namespace MathUtils {
	constexpr float Lerp(float start, float end, float percent)
	{
		return (start + percent * (end - start));
	}

	constexpr float Normalize(float value, float min, float max){
		/* https://stats.stackexchange.com/questions/70801/how-to-normalize-data-to-0-1-range */
		return (value - min) / (max - min);
	};

	float Abs(float value)
	{
		return std::abs(value);
	}
}