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
	if (IsEmpty())  return false;
	return SearchFor(n->index, 0);
}

bool NodeMinHeap::SearchFor(blaze::StaticVector<int, 3UL> value, int current_index) {
	if (data.at(current_index)->index == value) {
		return true;
	}
	bool l = false;
	if (LChild(current_index) < data.size()) {
		l = SearchFor(value, LChild(current_index));
	}
	bool r = false;
	if (RChild(current_index) < data.size()) {
		r = SearchFor(value, RChild(current_index));
	}
	return  l || r;
}

PGraphNode * NodeMinHeap::Remove()
{
	PGraphNode* result = data.at(0);
	Swap(0, data.size() - 1);
	data.pop_back();
	DownHeapBubble(0);
	return result;
}

bool NodeMinHeap::IsEmpty()
{
	return data.size() == 0;
}

