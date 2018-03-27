#include "stdafx.h"
#include "Obstacle.h"

void Obstacle::Draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 1);

	cairo_rectangle(cr, x, y, width, height);
	cairo_stroke(cr);
}

Obstacle::Obstacle(float x, float y, float width, float height)
{
	this->x = x - (width/2);
	this->y = y - (height/2);
	this->width = width;
	this->height = height;
}

Obstacle::~Obstacle()
{
}
