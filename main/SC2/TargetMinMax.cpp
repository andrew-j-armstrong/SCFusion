#include "stdafx.h"
#include "TargetMinMax.h"

template<>
bool CTargetMinMax<bool>::IsWithinRange(bool value) const
{
	return min <= value && value <= max;
}
