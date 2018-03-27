#include "stdafx.h"
#include "DrawDebug.h"


void DrawDebug::Draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 1, 0, 1);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 10, 0, 2 * 3.14);
	cairo_stroke(cr);
}

DrawDebug::DrawDebug(float x, float y)
{
	this->x = x;
	this->y = y;
}

DrawDebug::~DrawDebug()
{
}
