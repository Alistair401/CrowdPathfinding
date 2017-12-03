#include "stdafx.h"
#include "PGraph.h"
#include "blaze\Blaze.h"

struct IndexVectorEquals {
	bool operator()(const blaze::StaticVector<int, 3UL>& a, const blaze::StaticVector<int, 3UL>& b) const {
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
	}
};

struct HashIndexVector {
	// TODO: hash a blaze vector
};

PGraph::PGraph(blaze::StaticVector<float, 3UL> origin, blaze::StaticVector<int, 3UL> dimensions, float scale)
{
	// TODO: contruct graph
}

PGraph::~PGraph()
{
	// TODO: free graph
}
