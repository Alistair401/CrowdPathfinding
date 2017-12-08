#pragma once
#include "Drawable.h"
class Unit : public Drawable
{
public:
	Unit(double x, double y);
	~Unit();
	void draw(cairo_t* cr);
private:
	double x;
	double y;
};

