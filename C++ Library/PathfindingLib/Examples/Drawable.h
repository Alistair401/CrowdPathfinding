#pragma once
#include "cairo\cairo.h"

class Drawable
{
public:
	virtual void draw(cairo_t *cr) = 0;
};

