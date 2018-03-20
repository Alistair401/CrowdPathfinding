#include "stdafx.h"
#include "Agent.h"
#include <math.h>

Agent::Agent(float x, float y)
{
	this->x = x;
	this->y = y;
	system_id = PSystem::GetInstance().CreateUnit(Vector3{x, y, 0}, 0);
	SetTarget(x, y);
}

void Agent::Draw(cairo_t * cr)
{
	if (complete) return;
	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 5, 0, 2 * 3.14);
	cairo_stroke(cr);
}

Agent::~Agent()
{
	if (complete) return;
	PSystem::GetInstance().DestroyUnit(system_id);
}

void Agent::AddForce(float x, float y) {
	// assumes mass = 1 and delta_time = 1 since we're not too bothered about the phyiscs
	vel += Vector2{ x, y };
}

void Agent::Update()
{
	if (complete) return;
	float sqr_target_distance = SqrLength(Vector2{x, y} - target);
	if (sqr_target_distance < 80.0) {
		complete = true;
		PSystem::GetInstance().DestroyUnit(system_id);
	}
	else {
		UpdateForces();
		UpdateVelocity();
	}
}

void Agent::SetTarget(float x, float y)
{
	target = Vector2{ x, y };
	PSystem::GetInstance().UpdateUnitTarget(system_id, Vector3{ x, y, 0 });
}

bool Agent::IsComplete()
{
	return complete;
}

void Agent::UpdateVelocity() {
	if (glm::length(vel) > 1.0) {
		vel = glm::normalize(vel);
	}
	this->x += vel.x;
	this->y += vel.y;
	PSystem::GetInstance().UpdateUnitPosition(system_id, Vector3{x, y, 0});
}

void Agent::UpdateForces() {
	Vector3& force = PSystem::GetInstance().GetUnitForce(system_id);
	AddForce(force.x, force.y);
}
