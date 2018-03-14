#pragma once
#include "Drawable.h"
class Obstacle : public Drawable
{
public:
	void Draw(cairo_t * cr) override;
	Obstacle(float x, float y, float width, float height);
	~Obstacle();
private:
	float width;
	float height;
};

