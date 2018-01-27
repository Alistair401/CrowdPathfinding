#pragma once
#include "PUnit.h"
#include <vector>
#include <unordered_map>
#include "blaze\Blaze.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<double, 3, bg::cs::cartesian> point_t;
typedef std::pair<point_t, PUnit*> value_t;

class PUnitLayer
{
public:
	unsigned int id;
	void AddUnit(PUnit*, unsigned int unit_id);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id);
	std::vector<PUnit*> Nearby(unsigned int unit_id, double radius);
	PUnit* GetUnit(unsigned int unit_id);
private:
	bgi::rtree< value_t, bgi::quadratic<10> > rtree;
	//bgi::rtree< value_t, bgi::rstar<10> > rtree;
	std::unordered_map<unsigned int, value_t> members;
};

