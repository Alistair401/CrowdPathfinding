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
	bool operator==(const blaze::StaticVector<int, 3UL>& other) const {
		return index.at(0) == other.at(0) && index.at(1) == other.at(1) && index.at(2) == other.at(2);
	}

};
