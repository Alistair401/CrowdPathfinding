#include "stdafx.h"
#include "PUnit.h"

PUnit::PUnit(blaze::StaticVector<double, 3> position)
{
	this->position = position;
	this->heading = blaze::StaticVector<double, 3>{ 0,0,0 };
}

void PUnit::UpdatePosition(blaze::StaticVector<double, 3> position)
{
	this->position = position;
}

void PUnit::UpdateHeading(blaze::StaticVector<double, 3> heading)
{
	this->heading = heading;
}

void PUnit::UpdateTarget(blaze::StaticVector<double, 3> target)
{
	this->target = target;
}

void PUnit::SetLeader(unsigned int leader_id)
{
	this->leader = leader_id;
}

blaze::StaticVector<double, 3> PUnit::GetPosition()
{
	return position;
}

blaze::StaticVector<double, 3> PUnit::GetHeading()
{
	return heading;
}

blaze::StaticVector<double, 3> PUnit::GetTarget()
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