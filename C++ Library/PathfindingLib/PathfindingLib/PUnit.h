#pragma once
#include "blaze\Blaze.h"

class PUnit
{
public:
	PUnit(blaze::StaticVector<float, 3>& position);
	~PUnit();
	void UpdatePosition(blaze::StaticVector<float, 3>& position);
	void UpdateTarget(blaze::StaticVector<float, 3>& target);
	blaze::StaticVector<float, 3>& GetPosition();
	blaze::StaticVector<float, 3>& GetTarget();
private:
	blaze::StaticVector<float, 3> position;
	blaze::StaticVector<float, 3> target;
};
