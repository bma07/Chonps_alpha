#include "cepch.h"
#include "Math.h"

#include "Config.h"

namespace Chonps
{
	float radians(float degrees)
	{
		return degrees * (float)CHONPS_RAD;
	}
	
	float degrees(float radians)
	{
		return radians * (float)(180 / CHONPS_PI);
	}
}