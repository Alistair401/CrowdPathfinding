#pragma once
#include "blaze\Blaze.h"
#include "boost\functional\hash.hpp"

class PGraphNode
{
public:
	struct IndexHash
	{
		std::size_t operator()(blaze::StaticVector<int, 3UL> const& i) const noexcept
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, i.at(0));
			boost::hash_combine(seed, i.at(1));
			boost::hash_combine(seed, i.at(2));
			return seed;
		}
	};


	blaze::StaticVector<double, 3UL> position;
	blaze::StaticVector<int, 3UL> index;
	boolean obstacle;
	std::vector<PGraphNode*> neighbors;
	bool operator==(PGraphNode& other) const {
		return this->index == other.index;
	}
};
