#pragma once
#include "blaze\Blaze.h"

class PGraphNode
{
public:
	PGraphNode();
	~PGraphNode();
	blaze::StaticVector<double, 3UL> position;
	blaze::StaticVector<int, 3UL> index;
	boolean obstacle;
	std::vector<PGraphNode*> neighbors;
	bool operator==(const PGraphNode&) const;
	struct VectorEquals {
		bool operator()(const blaze::StaticVector<int, 3UL>& a, const blaze::StaticVector<int, 3UL>& b) const {
			return a == b;
		}
	};

};

