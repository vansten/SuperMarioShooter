#ifndef UTILITY_H
#define UTILITY_H

#include <math.h>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define sign(a) (a > 0 ? 1 : (a < 0 ? -1 : 0))
#define clamp(a, min_val, max_val) min(max(a, min_val), max_val)
#define acosM(x) (-0.69813170079773212 * x * x - 0.87266462599716477) * x + 1.5707963267948966
#define PI 3.14159f
#define DEG2RAD PI / 180.0f
#define RAD2DEG 180.0f / PI

typedef struct
{
	f32 x;
	f32 y;
} UV;

UV GetUV(f32 x, f32 y)
{
	UV uv;
	uv.x = x;
	uv.y = y;
	return uv;
}

f32 arctan2(f32 y, f32 x)
{
	f32 sqrtXY = sqrt(x * x + y * y);
	if(x > 0.0f)
	{
		return 2.0f * atan(y / (sqrtXY + x)) * RAD2DEG;
	}
	else
	{
		if(y == 0.0f)
		{
			if(x == 0.0f)
			{
				return 0.0f;
			}
			
			return PI * RAD2DEG;
		}
		else
		{
			return 2.0f * atan((sqrtXY - x) / y) * RAD2DEG;
		}
	}
	
	return 0.0f;
}

#endif