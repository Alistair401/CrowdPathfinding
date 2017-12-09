#include "stdafx.h"
#include "Unit.h"
#include <math.h>

Unit::Unit(double x, double y)
{
	this->x = x;
	this->y = y;
}

void Unit::draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 5, 0, 2 * 3.14);
	cairo_stroke(cr);
}

Unit::~Unit()
{
}

void Unit::update()
{
}


