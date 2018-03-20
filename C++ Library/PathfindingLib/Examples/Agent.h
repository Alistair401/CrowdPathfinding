#pragma once
#include "Drawable.h"
#include <vector>
#include <PSystem.h>

class Agent : public Drawable
{
public:
	Agent(float x, float y);
	void Draw(cairo_t* cr) override;
	void AddForce(float x, float y);
	void Update();
	void SetTarget(float x, float y);
	bool IsComplete();
	~Agent();
private:
	bool complete = false;
	unsigned int system_id;
	Vector2 target{ 0,0 };
	Vector2 vel{ 0,0 };
	void UpdateVelocity();
	void UpdateForces();
};

