#pragma once
#include "Drawable.h"
class Target :
	public Drawable
{
public:
	Target(double x, double y);
	~Target();
	void draw(cairo_t* cr);
};

