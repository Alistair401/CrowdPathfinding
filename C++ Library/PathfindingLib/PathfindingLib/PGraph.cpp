#include "stdafx.h"
#include "PGraph.h"

void add_edge(PGraphNode* a, PGraphNode* b) {
	a->neighbors.push_back(b);
	b->neighbors.push_back(a);
}

PGraphNode* PGraph::get_node_at(blaze::StaticVector<double, 3UL> position) {
	int max_x_index = (dimensions[0] / scale);
	int max_y_index = (dimensions[1] / scale);
	int max_z_index = (dimensions[2] / scale);

	int x_index = (std::clamp<double>(position[0], origin[0], origin[0] + dimensions[0]) / dimensions[0]) * max_x_index;
	int y_index = (std::clamp<double>(position[1], origin[1], origin[1] + dimensions[1]) / dimensions[1]) * max_y_index;
	int z_index = (std::clamp<double>(position[2], origin[2], origin[2] + dimensions[2]) / dimensions[2]) * max_z_index;
	return graph->at(blaze::StaticVector<int, 3>{x_index, y_index, z_index});
}

PGraph::PGraph(blaze::StaticVector<double, 3UL> origin, blaze::StaticVector<double, 3UL> dimensions, double scale)
{
	this->dimensions = dimensions;
	this->origin = origin;
	this->scale = scale;
	this->graph = new std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, IndexVectorHash, IndexVectorEquals>();
	int x_index = 0;
	int	y_index = 0;
	int z_index = 0;
	for (double i = origin[0]; i <= dimensions[0] + origin[0]; i += scale)
	{
		for (double j = origin[1]; j <= dimensions[1] + origin[1]; j += scale)
		{
			for (double k = origin[2]; k <= dimensions[2] + origin[2]; k += scale)
			{
				PGraphNode* new_node = new PGraphNode();
				blaze::StaticVector<int, 3UL> index{ x_index, y_index, z_index };
				new_node->position = blaze::StaticVector<double, 3UL>{ i,j,k };
				if (x_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3UL> { x_index - 1, y_index, z_index }));
				}
				if (y_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3UL> { x_index, y_index - 1, z_index }));
				}
				if (z_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3UL> { x_index, y_index, z_index - 1 }));
				}
				this->graph->insert(std::make_pair(index, new_node));
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
	if (this->graph) {
		for (auto it = this->graph->begin(); it != this->graph->end(); it++)
		{
			delete it->second;
		}
		delete this->graph;
	}
}

