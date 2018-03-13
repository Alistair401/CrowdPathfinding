#pragma once
#include "Common.h"

class PUnit
{
public:
	PUnit(Vector3& position);
	~PUnit();
	void UpdatePosition(Vector3& position);
	void UpdateTarget(Vector3& target);
	Vector3& GetPosition();
	Vector3& GetTarget();
private:
	Vector3 position;
	Vector3 target;
};
