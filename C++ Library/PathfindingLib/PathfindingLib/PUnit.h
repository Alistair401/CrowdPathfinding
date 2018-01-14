#pragma once
#include "blaze\Blaze.h"

class PUnit
{
public:
	blaze::StaticVector<float, 3UL> position;
	void UpdatePosition(blaze::StaticVector<float, 3UL> position);
};
