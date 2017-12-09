#pragma once
#include "Drawable.h"
class Unit : public Drawable
{
public:
	void draw(cairo_t* cr) override;
	Unit(double x, double y);
	~Unit();
	void update();
};

