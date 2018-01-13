#pragma once
#include "cairo\cairo.h"

class Drawable
{
public:
	virtual void Draw(cairo_t *cr) = 0;
	double x;
	double y;
};

