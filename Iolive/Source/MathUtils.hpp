#pragma once

namespace MathUtils {
	constexpr float Lerp(float start, float end, float percent)
	{
		return (start + percent * (end - start));
	}
}