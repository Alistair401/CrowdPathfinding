#pragma once
#include "PGraphNode.h"
#include <vector>
#include <unordered_map>

class NodeMinHeap
{
public:
	NodeMinHeap(std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash>* f_score_map);
	std::vector<PGraphNode*> data;
	std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash>* f_score;

	void Insert(PGraphNode*);
	bool Contains(PGraphNode*);
	bool SearchFor(blaze::StaticVector<int, 3UL> value, int root);
	PGraphNode* Remove();
private:
	void Swap(int a, int b);
	void UpHeapBubble(int index);
	void DownHeapBubble(int index);
};

