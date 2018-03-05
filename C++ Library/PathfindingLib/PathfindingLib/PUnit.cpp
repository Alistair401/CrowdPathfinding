#include "stdafx.h"
#include "PUnit.h"

PUnit::PUnit(blaze::StaticVector<float, 3>& position)
{
	this->position = position;
}

void PUnit::UpdatePosition(blaze::StaticVector<float, 3>& position)
{
	this->position = position;
}

void PUnit::UpdateTarget(blaze::StaticVector<float, 3>& target)
{
	this->target = target;
}

blaze::StaticVector<float, 3>& PUnit::GetPosition()
{
	return position;
}

blaze::StaticVector<float, 3>& PUnit::GetTarget()
{
	return target;
}

PUnit::~PUnit()
{
}
