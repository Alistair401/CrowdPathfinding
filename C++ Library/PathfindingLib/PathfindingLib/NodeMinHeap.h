#pragma once
#include "Common.h"
#include "PGraphNode.h"
#include <vector>
#include <unordered_map>

class NodeMinHeap
{
public:
	NodeMinHeap(std::unordered_map<IVector3, float, IVector3Hash>* f_score_map);
	std::unordered_map<IVector3, float, IVector3Hash>* f_score;

	void Insert(PGraphNode*);
	bool Contains(PGraphNode*);
	PGraphNode* Remove();
	bool IsEmpty();
	void Update(PGraphNode*);
private:
	std::vector<PGraphNode*> data;
	void Swap(int a, int b);
	void UpHeapBubble(int index);
	void DownHeapBubble(int index);
	int SearchFor(IVector3 value, int root);
	bool IsHeap(int current_index);
};

