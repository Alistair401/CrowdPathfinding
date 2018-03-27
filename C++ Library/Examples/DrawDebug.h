#pragma once
#include "Drawable.h"
class DrawDebug :
	public Drawable
{
public:
	void Draw(cairo_t * cr) override;
	DrawDebug(float x, float y);
	~DrawDebug();
};

