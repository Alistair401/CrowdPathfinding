#pragma once
#include "blaze\Blaze.h"

class PUnit
{
public:
	PUnit(blaze::StaticVector<float, 3> position);
	~PUnit();
	void UpdatePosition(blaze::StaticVector<float, 3> position);
	void UpdateHeading(blaze::StaticVector<float, 3> heading);
	void UpdateTarget(blaze::StaticVector<float, 3> target);
	void SetLeader(unsigned int leader_id);
	unsigned int GetLeader();
	blaze::StaticVector<float, 3> GetPosition();
	blaze::StaticVector<float, 3> GetHeading();
	blaze::StaticVector<float, 3> GetTarget();
private:
	blaze::StaticVector<float, 3> position;
	blaze::StaticVector<float, 3> heading;
	blaze::StaticVector<float, 3> target;
	unsigned int leader = 0;
};
