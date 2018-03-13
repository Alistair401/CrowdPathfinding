#pragma once
#include "blaze\Blaze.h"
#include "boost\functional\hash.hpp"

using Vector3 = blaze::StaticVector<float, 3>;
using IVector3 = blaze::StaticVector<int, 3>;

struct IVector3Hash
{
	std::size_t operator()(IVector3 const& i) const noexcept
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, i.at(0));
		boost::hash_combine(seed, i.at(1));
		boost::hash_combine(seed, i.at(2));
		return seed;
	}
};