#pragma once
#include "Drawable.h"
#include "blaze\Blaze.h"
#include <vector>

class Unit : public Drawable
{
public:
	Unit(float x, float y);
	void Draw(cairo_t* cr) override;
	void AddForce(float x, float y);
	void Update();
	void SetTarget(float x, float y);
	bool IsComplete();
	~Unit();
private:
	bool complete = false;
	unsigned int system_id;
	blaze::StaticVector<float, 2> target{ 0,0 };
	blaze::StaticVector<float, 2> vel{ 0,0 };
	void UpdateVelocity();
	void UpdateForces();
};

