#include "stdafx.h"
#include "PUnit.h"

PUnit::PUnit(blaze::StaticVector<float, 3> position)
{
	this->position = position;
	this->heading = blaze::StaticVector<float, 3>{ 0,0,0 };
}

void PUnit::UpdatePosition(blaze::StaticVector<float, 3> position)
{
	this->position = position;
}

void PUnit::UpdateHeading(blaze::StaticVector<float, 3> heading)
{
	this->heading = heading;
}

void PUnit::UpdateTarget(blaze::StaticVector<float, 3> target)
{
	this->target = target;
}

void PUnit::SetLeader(unsigned int leader_id)
{
	this->leader = leader_id;
}

blaze::StaticVector<float, 3> PUnit::GetPosition()
{
	return position;
}

blaze::StaticVector<float, 3> PUnit::GetHeading()
{
	return heading;
}

blaze::StaticVector<float, 3> PUnit::GetTarget()
{
	return target;
}

unsigned int PUnit::GetLeader()
{
	return leader;
}

PUnit::~PUnit()
{
}
