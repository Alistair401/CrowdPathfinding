#include "stdafx.h"
#include "PUnit.h"
#include "UnitMediator.h"

PUnit::PUnit(blaze::StaticVector<double, 3UL> position)
{
	this->mediator = &UnitMediator::GetInstance();
	this->id = mediator->AddUnit(this,0);
	this->position = position;
	this->heading = blaze::StaticVector<double, 3>{ 0,0,0 };
}

void PUnit::UpdatePosition(blaze::StaticVector<double, 3> position)
{
	this->position = position;
	this->mediator->UpdateUnit(id, layer_id, position);
}

void PUnit::UpdateHeading(blaze::StaticVector<double, 3UL> heading)
{
	this->heading = heading;
}

void PUnit::UpdateTarget(blaze::StaticVector<double, 3UL> target)
{
	this->target = target;
}

blaze::StaticVector<double, 3> PUnit::GetForce()
{
	return mediator->GetForce(layer_id, id);
}

void PUnit::SetLayer(unsigned int layer_id)
{
	printf("ERROR: Layer changing not yet implemented");
}

void PUnit::SetLeader(PUnit * unit)
{
	this->leader = unit;
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

PUnit * PUnit::GetLeader()
{
	return leader;
}

PUnit::~PUnit()
{
	mediator->RemoveUnit(id,layer_id);
}