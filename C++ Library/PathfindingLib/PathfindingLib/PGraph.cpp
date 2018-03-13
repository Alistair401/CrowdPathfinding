#include "stdafx.h"
#include "PGraph.h"

void add_edge(PGraphNode* a, PGraphNode* b) {
	a->neighbors.push_back(b);
	b->neighbors.push_back(a);
}

PGraphNode* PGraph::NodeAt(Vector3& position) {
	int max_x_index = static_cast<int>(std::floor(dimensions[0] / scale));
	int max_y_index = static_cast<int>(std::floor(dimensions[1] / scale));
	int max_z_index = static_cast<int>(std::floor(dimensions[2] / scale));

	int x_index = 0, y_index = 0, z_index = 0;

	if (dimensions[0] > 0) {
		float x_proportion = std::clamp((position[0] - origin[0]) / dimensions[0], 0.0f, 1.0f);
		x_index = static_cast<int>(std::round(x_proportion * max_x_index));
	}

	if (dimensions[1] > 0) {
		float y_proportion = std::clamp((position[1] - origin[1]) / dimensions[1], 0.0f, 1.0f);
		y_index = static_cast<int>(std::round(y_proportion * max_y_index));
	}

	if (dimensions[2] > 0) {
		float z_proportion = std::clamp((position[2] - origin[2]) / dimensions[2], 0.0f, 1.0f);
		z_index = static_cast<int>(std::round(z_proportion * max_z_index));
	}

	return NodeAtIndex(IVector3{ x_index, y_index, z_index });
}

PGraphNode* PGraph::NodeAtIndex(IVector3& index) {
	return grid[index[0]][index[1]][index[2]];
}

PGraph::PGraph(Vector3& origin, Vector3& dimensions, float scale)
{
	this->origin = origin;
	this->scale = scale;

	int max_x_index = static_cast<int>(std::floor(dimensions[0] / scale));
	int max_y_index = static_cast<int>(std::floor(dimensions[1] / scale));
	int max_z_index = static_cast<int>(std::floor(dimensions[2] / scale));

	grid.resize(max_x_index + 1);

	int x_index = 0;
	for (float i = origin[0]; i <= dimensions[0] + origin[0]; i += scale)
	{
		grid[x_index].resize(max_y_index + 1);
		int	y_index = 0;
		for (float j = origin[1]; j <= dimensions[1] + origin[1]; j += scale)
		{
			grid[x_index][y_index].resize(max_z_index + 1);
			int z_index = 0;
			for (float k = origin[2]; k <= dimensions[2] + origin[2]; k += scale)
			{
				PGraphNode* new_node = new PGraphNode();
				IVector3 index{ x_index, y_index, z_index };
				new_node->position = Vector3{ i,j,k };
				new_node->index = IVector3{ x_index,y_index,z_index };
				bool min_x_edge = x_index == 0;
				bool min_y_edge = y_index == 0;
				bool min_z_edge = z_index == 0;
				bool max_x_edge = x_index == max_x_index;
				bool max_y_edge = y_index == max_y_index;
				bool max_z_edge = z_index == max_z_index;

				if (!min_x_edge) {
					if (!max_y_edge) {
						if (!min_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index + 1, z_index - 1 }));
						add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index + 1, z_index }));
						if (!max_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index + 1, z_index + 1 }));
					}
					if (!min_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index, z_index - 1 }));
					add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index, z_index }));
					if (!max_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index, z_index + 1 }));
					if (!min_y_edge) {
						if (!min_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index - 1, z_index - 1 }));
						add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index - 1, z_index }));
						if (!max_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index - 1, y_index - 1, z_index + 1 }));
					}
				}
				if (!min_y_edge) {
					if (!min_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index, y_index - 1, z_index - 1 }));
					add_edge(new_node, NodeAtIndex(IVector3{ x_index, y_index - 1, z_index }));
					if (!max_z_edge) add_edge(new_node, NodeAtIndex(IVector3{ x_index, y_index - 1, z_index + 1 }));
				}
				if (!min_z_edge) {
					add_edge(new_node, NodeAtIndex(IVector3{ x_index, y_index, z_index - 1 }));
				}

				grid[x_index][y_index][z_index] = new_node;
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
	//if (this->graph) {
	//	for (auto it = this->graph->begin(); it != this->graph->end(); it++)
	//	{
	//		delete it->second;
	//	}
	//	delete this->graph;
	//}
}

