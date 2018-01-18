#pragma once
#include "Drawable.h"
#include "blaze\Blaze.h"
#include "PGraph.h"
#include "PUnit.h"
#include <vector>

class Unit : public Drawable
{
public:
	Unit(double x, double y);
	void Draw(cairo_t* cr) override;
	void AddForce(double x, double y);
	void Update(PGraph* graph);
	void SetTarget(double x, double y);
	std::vector<blaze::StaticVector<double, 3UL>>* path = 0;
	~Unit();
private:
	PUnit * unit;
	blaze::StaticVector<double, 2UL> target{ 0,0 };
	blaze::StaticVector<double, 2UL> vel{ 0,0 };
	unsigned int path_index = 0;
	void UpdateVelocity();
	void UpdateForces();
	void UpdatePath(PGraph* graph);
};

