#pragma once
#include "Drawable.h"
#include "blaze\Blaze.h"
#include <vector>

class Unit : public Drawable
{
public:
	Unit(double x, double y);
	void Draw(cairo_t* cr) override;
	void AddForce(double x, double y);
	void Update();
	void SetTarget(double x, double y);
	~Unit();
private:
	unsigned int system_id;
	blaze::StaticVector<double, 2> target{ 0,0 };
	blaze::StaticVector<double, 2> vel{ 0,0 };
	void UpdateVelocity();
	void UpdateForces();
};

