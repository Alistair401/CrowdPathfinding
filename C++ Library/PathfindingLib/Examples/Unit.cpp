#include "stdafx.h"
#include "Unit.h"
#include "Pathfinding.h"
#include <math.h>

Unit::Unit(double x, double y)
{
	this->x = x;
	this->y = y;
	unit = new PUnit(blaze::StaticVector<double, 3>{x, y, 0});
	SetTarget(x, y);
}

void Unit::Draw(cairo_t * cr)
{
	cairo_set_source_rgb(cr, 0, 0, 1);
	cairo_set_line_width(cr, 1);

	cairo_arc(cr, x, y, 5, 0, 2 * 3.14);
	cairo_stroke(cr);
}

Unit::~Unit()
{
}

void Unit::AddForce(double x, double y) {
	// assumes mass = 1 and time = 1 since we're not too bothered about the phyiscs
	vel[0] += x;
	vel[1] += y;
}

void Unit::Update(PGraph* graph)
{
	UpdateForces();
	UpdateVelocity();
	//UpdatePath(graph);
}

void Unit::SetTarget(double x, double y)
{
	target[0] = x;
	target[1] = y;
}

void Unit::UpdateVelocity() {
	if (blaze::length(vel) > 1.0) {
		vel = blaze::normalize(vel);
	}
	this->x += vel.at(0);
	this->y += vel.at(1);
	unit->UpdatePosition(blaze::StaticVector<double, 3>{x, y, 0});
	unit->UpdateHeading(blaze::StaticVector<double, 3>{vel.at(0), vel.at(1), 0});
}

void Unit::UpdateForces() {
	blaze::StaticVector<double, 3> force = unit->GetForce();
	AddForce(force.at(0), force.at(1));
}

void Unit::UpdatePath(PGraph* graph) {
	if (!path) {
		path_index = 0;
		path = Pathfinding::a_star(graph, blaze::StaticVector<double, 3>{x, y, 0.0f}, blaze::StaticVector<double, 3>{target.at(0), target.at(1), 0.0});
	}
	blaze::StaticVector<double, 3UL> current_path = path->at(path_index);
	// check if we're close enough to the current path_index to move to the next one, but don't exceed the path length
	while (std::sqrt(std::pow(x - current_path[0], 2) + std::pow(y - current_path[1], 2)) < 20) {
		if (path_index == path->size() - 1) break;
		path_index++;
		current_path = path->at(path_index);
	}
	// add a force in the direction of the next path_index
	double dir_x = current_path[0] - x;
	double dir_y = current_path[1] - y;
	AddForce(dir_x, dir_y);
}
