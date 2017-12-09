#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<double, 3UL> origin, blaze::StaticVector<double, 3UL> dimensions, double scale);
	~PGraph();
private:
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
	std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, IndexVectorHash, IndexVectorEquals>* graph;
	blaze::StaticVector<double, 3UL> dimensions;
	blaze::StaticVector<double, 3UL> origin;
	double scale;
	PGraphNode* PGraph::get_node_at(blaze::StaticVector<double, 3UL> position);
};

