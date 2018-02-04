#include "stdafx.h"
#include "PGraph.h"

void add_edge(PGraphNode* a, PGraphNode* b) {
	a->neighbors.push_back(b);
	b->neighbors.push_back(a);
}

PGraphNode* PGraph::NodeAt(blaze::StaticVector<double, 3> position) {
	int max_x_index = static_cast<int>(std::floor(dimensions[0] / scale));
	int max_y_index = static_cast<int>(std::floor(dimensions[1] / scale));
	int max_z_index = static_cast<int>(std::floor(dimensions[2] / scale));

	int x_index = 0, y_index = 0, z_index = 0;

	if (dimensions[0] > 0) {
		double x_proportion = std::clamp((position[0] - origin[0]) / dimensions[0], 0.0, 1.0);
		x_index = static_cast<int>(std::round(x_proportion * max_x_index));
	}

	if (dimensions[1] > 0) {
		double y_proportion = std::clamp((position[1] - origin[1]) / dimensions[1], 0.0, 1.0);
		y_index = static_cast<int>(std::round(y_proportion * max_y_index));
	}

	if (dimensions[2] > 0) {
		double z_proportion = std::clamp((position[2] - origin[2]) / dimensions[2], 0.0, 1.0);
		z_index = static_cast<int>(std::round(z_proportion * max_z_index));
	}

	return graph->at(blaze::StaticVector<int, 3>{x_index, y_index, z_index});
}

PGraph::PGraph(blaze::StaticVector<double, 3> origin, blaze::StaticVector<double, 3> dimensions, double scale)
{
	this->origin = origin;
	this->scale = scale;
	this->graph = new std::unordered_map<blaze::StaticVector<int, 3>, PGraphNode*, PGraphNode::IndexHash>();
	int x_index = 0;
	for (double i = origin[0]; i <= dimensions[0] + origin[0]; i += scale)
	{
		int	y_index = 0;
		for (double j = origin[1]; j <= dimensions[1] + origin[1]; j += scale)
		{
			int z_index = 0;
			for (double k = origin[2]; k <= dimensions[2] + origin[2]; k += scale)
			{
				PGraphNode* new_node = new PGraphNode();
				blaze::StaticVector<int, 3> index{ x_index, y_index, z_index };
				new_node->position = blaze::StaticVector<double, 3UL>{ i,j,k };
				new_node->index = blaze::StaticVector<int, 3>{ x_index,y_index,z_index };
				if (x_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3> { x_index - 1, y_index, z_index }));
				}
				if (y_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3> { x_index, y_index - 1, z_index }));
				}
				if (z_index > 0) {
					add_edge(new_node, this->graph->at(blaze::StaticVector<int, 3> { x_index, y_index, z_index - 1 }));
				}
				this->graph->insert(std::make_pair(index, new_node));
				this->dimensions = { i - origin[0],j - origin[1],k - origin[2] };
				z_index++;
			}
			y_index++;
		}
		x_index++;
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

