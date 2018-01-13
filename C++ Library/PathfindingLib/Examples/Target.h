#pragma once
#include "Drawable.h"
class Target : public Drawable
{
public:
	void Draw(cairo_t * cr) override;
	Target(double x, double y);
	~Target();
};

