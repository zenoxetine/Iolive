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

	inline float Abs(float value)
	{
		return std::abs(value);
	}

	inline float Difference(float value1, float value2)
	{
		return std::abs(std::abs(value1) - std::abs(value2));
	}
}