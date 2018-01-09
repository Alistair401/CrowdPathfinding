#include "stdafx.h"
#include "NodeMinHeap.h"

NodeMinHeap::NodeMinHeap(std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash>* f_score_map)
{
	this->f_score = f_score_map;
}

int Parent(int index) {
	return (index - 1) / 2;
}

int LChild(int index) {
	return (index * 2) + 1;
}

int RChild(int index) {
	return (index * 2) + 2;
}

void NodeMinHeap::Swap(int a, int b) {
	std::iter_swap(data.begin() + a, data.begin() + b);
}

void NodeMinHeap::UpHeapBubble(int index) {
	int p_idx = Parent(index);
	if (p_idx >= 0 && f_score->at(data.at(p_idx)->index) < f_score->at(data.at(index)->index)) {
		Swap(p_idx, index);
		UpHeapBubble(p_idx);
	}
}

void NodeMinHeap::DownHeapBubble(int index) {
	int smallest = index;
	int lchild = LChild(index);
	int rchild = RChild(index);
	if (lchild < data.size() && f_score->at(data.at(lchild)->index) < f_score->at(data.at(smallest)->index)) {
		smallest = lchild;
	}
	if (rchild < data.size() && f_score->at(data.at(rchild)->index) < f_score->at(data.at(smallest)->index)) {
		smallest = rchild;
	}
	if (smallest != index) {
		Swap(smallest, index);
		DownHeapBubble(smallest);
	}
}

void NodeMinHeap::Insert(PGraphNode * n)
{
	data.push_back(n);
	UpHeapBubble(data.size() - 1);
}

bool NodeMinHeap::Contains(PGraphNode * n)
{
	return SearchFor(n->index, 0);
}

bool NodeMinHeap::SearchFor(blaze::StaticVector<int, 3UL> value, int root) {
	if (root >= data.size()) {
		return false;
	}
	if (data.at(root)->index == value) {
		return true;
	}
	if (f_score->at(data.at(root)->index) < f_score->at(value)) {
		return SearchFor(value, LChild(root)) || SearchFor(value, RChild(root));
	}
	return false;
}

PGraphNode * NodeMinHeap::Remove()
{
	PGraphNode* result = data.at(0);
	Swap(0, data.size() - 1);
	data.pop_back();
	DownHeapBubble(0);
	return result;
}

