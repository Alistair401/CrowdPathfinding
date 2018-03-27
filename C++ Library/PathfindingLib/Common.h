#pragma once
#include "glm\glm.hpp"
#include "boost\functional\hash.hpp"

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using IVector3 = glm::ivec3;

static float SqrLength(Vector3 const& vec) {
	return glm::dot(vec, vec);
}

static float SqrLength(Vector2 const& vec) {
	return glm::dot(vec, vec);
}

struct IVector3Hash
{
	std::size_t operator()(IVector3 const& i) const noexcept
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, i.x);
		boost::hash_combine(seed, i.y);
		boost::hash_combine(seed, i.z);
		return seed;
	}
};