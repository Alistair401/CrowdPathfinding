#pragma once
#include "Drawable.h"
#include "blaze\Blaze.h"
#include "PGraph.h"
#include <vector>

class Unit : public Drawable
{
public:
	void draw(cairo_t* cr) override;
	Unit(double x, double y);
	~Unit();
	void update(PGraph* graph);
private:
	std::vector<blaze::StaticVector<double, 3UL>>* path = 0;
};

