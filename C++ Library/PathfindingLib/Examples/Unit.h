#pragma once
#include "Drawable.h"
#include "blaze\Blaze.h"
#include "PGraph.h"

class Unit : public Drawable
{
public:
	void draw(cairo_t* cr) override;
	Unit(double x, double y);
	~Unit();
	void update(PGraph* graph);
private:
	blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>* path = 0;
};

