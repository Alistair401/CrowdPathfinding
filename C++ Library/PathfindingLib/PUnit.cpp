#include "stdafx.h"
#include "PUnit.h"

PUnit::PUnit(Vector3& position)
{
	this->position = position;
}

void PUnit::UpdatePosition(Vector3& position)
{
	this->position = position;
}

void PUnit::UpdateTarget(Vector3& target)
{
	this->target = target;
}

Vector3& PUnit::GetPosition()
{
	return position;
}

Vector3& PUnit::GetTarget()
{
	return target;
}

PUnit::~PUnit()
{
}
