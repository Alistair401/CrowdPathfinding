#include "stdafx.h"
#include "PGraph.h"

struct IndexVectorEquals {
	bool operator()(const blaze::StaticVector<int, 3UL>& a, const blaze::StaticVector<int, 3UL>& b) const {
		return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
	}
};

struct IndexVectorHash {
	std::size_t operator()(const blaze::StaticVector<int, 3UL>& k)const {
		size_t x_hash = std::hash<int>()(k[0]);
		size_t y_hash = std::hash<int>()(k[1]);
		size_t z_hash = std::hash<int>()(k[2]);
		return (x_hash ^ (y_hash << 1)) ^ z_hash;
	}
};

std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, IndexVectorHash, IndexVectorEquals>* graph = new std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, IndexVectorHash, IndexVectorEquals>();

PGraph::PGraph(blaze::StaticVector<double, 3UL> origin, blaze::StaticVector<int, 3UL> dimensions, double scale)
{
	int x_index = 0;
	int	y_index = 0;
	int z_index = 0;
	for (double i = origin[0] + (scale / 2.0); i < dimensions[0] + origin[0]; i += scale)
	{
		for (double j = origin[1] + (scale / 2.0); j < dimensions[1] + origin[1]; j += scale)
		{
			for (double k = origin[2] + (scale / 2.0); k < dimensions[2] + origin[2]; k += scale)
			{
				PGraphNode* new_node = new PGraphNode();
				blaze::StaticVector<int, 3UL> index { x_index, y_index, z_index };
				graph->insert(std::make_pair(index,new_node));
				z_index++;
			}
			y_index++;
			z_index = 0;
		}
		x_index++;
		y_index = 0;
	}
}

PGraph::~PGraph()
{
	// TODO: free graph
}