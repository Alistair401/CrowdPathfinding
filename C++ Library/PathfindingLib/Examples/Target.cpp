#include "stdafx.h"
#include "Target.h"


Target::Target(double x, double y)
{
	this->x = x;
	this->y = y;
}

Target::~Target()
{
}

void Target::Draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 1, 0, 0);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 3, 0, 2 * 3.14);
	cairo_fill(cr);
}
