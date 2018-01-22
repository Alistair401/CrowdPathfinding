#pragma once
#include "blaze\Blaze.h"

class UnitMediator;

class PUnit
{
public:
	PUnit(blaze::StaticVector<double, 3UL> position);
	~PUnit();
	void UpdatePosition(blaze::StaticVector<double, 3UL> position);
	void UpdateHeading(blaze::StaticVector<double, 3UL> heading);
	void UpdateTarget(blaze::StaticVector<double, 3UL> target);
	void SetLayer(unsigned int);
	void SetLeader(PUnit* unit);
	blaze::StaticVector<double, 3> GetForce();
	blaze::StaticVector<double, 3> GetPosition();
	blaze::StaticVector<double, 3> GetHeading();
	blaze::StaticVector<double, 3> GetTarget();
	PUnit* GetLeader();
private:
	UnitMediator * mediator;
	unsigned int id;
	unsigned int layer_id = 0;
	blaze::StaticVector<double, 3> position;
	blaze::StaticVector<double, 3> heading;
	blaze::StaticVector<double, 3> target;
	PUnit* leader = nullptr;
};
