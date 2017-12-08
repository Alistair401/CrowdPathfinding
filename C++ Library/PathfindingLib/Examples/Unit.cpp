#include "stdafx.h"
#include "Unit.h"
#include <math.h>

double x;
double y;

Unit::Unit(double x, double y)
{
	this->x = x;
	this->y = y;
	printf("Created Unit at (%f,%f)\n", x, y);
}

Unit::~Unit()
{
}


void Unit::draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 0.69, 0.19, 0);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 5, 0, 2 * 3.14);
	cairo_stroke(cr);
	//cairo_stroke_preserve(cr);
	//cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
	//cairo_fill(cr);
}


