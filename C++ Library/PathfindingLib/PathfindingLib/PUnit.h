#pragma once
#include "blaze\Blaze.h"

class PUnit
{
public:
	PUnit(blaze::StaticVector<double, 3> position);
	~PUnit();
	void UpdatePosition(blaze::StaticVector<double, 3> position);
	void UpdateHeading(blaze::StaticVector<double, 3> heading);
	void UpdateTarget(blaze::StaticVector<double, 3> target);
	void SetLeader(unsigned int leader_id);
	unsigned int GetLeader();
	blaze::StaticVector<double, 3> GetPosition();
	blaze::StaticVector<double, 3> GetHeading();
	blaze::StaticVector<double, 3> GetTarget();
private:
	blaze::StaticVector<double, 3> position;
	blaze::StaticVector<double, 3> heading;
	blaze::StaticVector<double, 3> target;
	unsigned int leader = 0;
};
