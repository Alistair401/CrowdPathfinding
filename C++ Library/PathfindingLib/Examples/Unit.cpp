#include "stdafx.h"
#include "Unit.h"
#include "Pathfinding.h"
#include <math.h>
#include <PSystem.h>

Unit::Unit(float x, float y)
{
	this->x = x;
	this->y = y;
	system_id = PSystem::GetInstance().CreateUnit(blaze::StaticVector<float, 3>{x, y, 0}, 0);
	SetTarget(x, y);
}

void Unit::Draw(cairo_t * cr)
{
	if (complete) return;
	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 5, 0, 2 * 3.14);
	cairo_stroke(cr);
}

Unit::~Unit()
{
	if (complete) return;
	PSystem::GetInstance().DestroyUnit(system_id);
}

void Unit::AddForce(float x, float y) {
	// assumes mass = 1 and delta_time = 1 since we're not too bothered about the phyiscs
	vel += blaze::StaticVector<float, 2>{ x, y };
}

void Unit::Update()
{
	if (complete) return;
	float sqr_target_distance = blaze::sqrLength(blaze::StaticVector<float, 2>{x, y} -target);
	if (sqr_target_distance < 49.0) {
		complete = true;
		PSystem::GetInstance().DestroyUnit(system_id);
	}
	else {
		UpdateForces();
		UpdateVelocity();
	}
}

void Unit::SetTarget(float x, float y)
{
	target = blaze::StaticVector<float, 2>{ x, y };
	PSystem::GetInstance().UpdateUnitTarget(system_id, blaze::StaticVector<float, 3>{ x, y, 0 });
}

void Unit::UpdateVelocity() {
	if (blaze::length(vel) > 1.0) {
		vel = blaze::normalize(vel);
	}
	this->x += vel.at(0);
	this->y += vel.at(1);
	PSystem::GetInstance().UpdateUnitPosition(system_id, blaze::StaticVector<float, 3>{x, y, 0});
}

void Unit::UpdateForces() {
	blaze::StaticVector<float, 3>& force = PSystem::GetInstance().GetUnitForce(system_id);
	AddForce(force.at(0), force.at(1));
}
