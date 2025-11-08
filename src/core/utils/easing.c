#pragma once

#include "../types.c"
#include "../raymath.h"

f32 EaseOutQuart(const f32 _t)
{
	return 1.f - pow(1.f - _t, 4.f);
}

f32 EaseOutCirc(const f32 _t)
{
	return sqrt(1.f - pow(_t - 1.f, 2.f));
}

f32 EaseInOutElastic(const f32 _t)
{
	const f32 c5 = (2.f * PI) / 4.5f;

	if (_t == 0.f)
	{
		return 0.f;
	}
	else
	{
		if (_t == 1.f)
		{
			return 1.f;
		}
		else
		{
			if (_t < 0.5f)
			{
				return -(pow(2.f, 20.f * _t - 10.f) * sinf((20.f * _t - 11.125f) * c5)) * 2.f;
			}
			else
			{
				return (pow(2.f, -20.f * _t + 10.f) * sinf((20.f * _t - 11.125f) * c5)) * 2.f + 1.f;
			}
		}
	}
}

f32 EaseInOutBack(f32 _t)
{
	const f32 c1 = 1.70158f;
	const f32 c2 = c1 * 1.525f;

	if (_t < 0.5f)
		return (powf(2.0f * _t, 2.0f) * ((c2 + 1.0f) * 2.0f * _t - c2)) / 2.0f;
	else
		return (powf(2.0f * _t - 2.0f, 2.0f) * ((c2 + 1.0f) * (2.0f * _t - 2.0f) + c2) + 2.0f) / 2.0f;
}

f32 EaseOutBounce(f32 x)
{
	const f32 n1 = 7.5625f;
	const f32 d1 = 2.75f;

	if (x < 1.0f / d1)
	{
		return n1 * x * x;
	}
	else if (x < 2.0f / d1)
	{
		x -= 1.5f / d1;
		return n1 * x * x + 0.75f;
	}
	else if (x < 2.5f / d1)
	{
		x -= 2.25f / d1;
		return n1 * x * x + 0.9375f;
	}
	else
	{
		x -= 2.625f / d1;
		return n1 * x * x + 0.984375f;
	}
}

f32 EaseCosH(const f32 _t)
{
	return (cosh(_t - 0.5)) * 7.835392328 - 7.835392328;
}