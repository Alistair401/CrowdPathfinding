#pragma once
#include "PUnit.h"
#include <vector>
#include "blaze\Blaze.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace geometry = boost::geometry;
namespace geometryindex = boost::geometry::index;

class PUnitLayer
{
public:
	unsigned int id;
	unsigned int AddUnit(PUnit*);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id, blaze::StaticVector<double, 3> position);
	std::vector<PUnit*> Nearby(unsigned int unit_id, double radius);
	PUnit* GetUnit(unsigned int unit_id);
private:
	// -- Spatial data -- 
	typedef geometry::model::point<double, 3, geometry::cs::cartesian> point;
	typedef boost::geometry::model::polygon<point> polygon;
	typedef std::pair<point, PUnit*> value;
	geometryindex::rtree< value, geometryindex::quadratic<16> > rtree;
	// ------------------
	std::vector<PUnit*> units;
};

