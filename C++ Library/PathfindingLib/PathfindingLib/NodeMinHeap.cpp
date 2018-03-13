#include "stdafx.h"
#include "NodeMinHeap.h"

NodeMinHeap::NodeMinHeap(std::unordered_map<IVector3, float, IVector3Hash>* f_score_map)
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
	int parent = Parent(index);
	if (parent >= 0 && f_score->at(data.at(parent)->index) > f_score->at(data.at(index)->index)) {
		Swap(parent, index);
		UpHeapBubble(parent);
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
	return SearchFor(n->index, 0) != -1;
}

int NodeMinHeap::SearchFor(IVector3 value, int current_index) {
	if (data.at(current_index)->index == value) {
		return current_index;
	}
	int l = -1;
	if (LChild(current_index) < data.size()) {
		l = SearchFor(value, LChild(current_index));
	}
	int r = -1;
	if (RChild(current_index) < data.size()) {
		r = SearchFor(value, RChild(current_index));
	}
	if (l == -1) {
		return r;
	}
	return  l;
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

void NodeMinHeap::Update(PGraphNode* item) {
	int index = SearchFor(item->index, 0);
	UpHeapBubble(index);
}

bool NodeMinHeap::IsHeap(int current_index) {
	if (current_index >= data.size()) {
		return true;
	}
	float max = f_score->at(data.at(current_index)->index);
	if (LChild(current_index) < data.size())
		max = std::max(max, f_score->at(data.at(LChild(current_index))->index));
	if (RChild(current_index) < data.size())
		max = std::max(max, f_score->at(data.at(RChild(current_index))->index));
	return max == f_score->at(data.at(current_index)->index) && IsHeap(LChild(current_index)) && IsHeap(RChild(current_index));
}

