#include "stdafx.h"
#include "NodeMinHeap.h"

NodeMinHeap::NodeMinHeap(std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash>* f_score_map)
{
	this->f_score = f_score_map;
}

int Parent(int index) {
	return (index - 1) / 2;
}

void NodeMinHeap::Swap(int a, int b) {
	std::iter_swap(data.begin() + a, data.begin() + b);
}

void NodeMinHeap::UpHeapBubble(int index) {
	int p_idx = Parent(index);
	if (p_idx > 0 && f_score->at(data.at(p_idx)->index) < f_score->at(data.at(index)->index)) {
		Swap(p_idx, index);
		UpHeapBubble(p_idx);
	}
}

void NodeMinHeap::Insert(PGraphNode * n)
{
	data.push_back(n);
	UpHeapBubble(data.size() - 1);
}

bool NodeMinHeap::Contains(PGraphNode * n)
{
	return false;
}

PGraphNode * NodeMinHeap::Remove()
{
	return nullptr;
}

