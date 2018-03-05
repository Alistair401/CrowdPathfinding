#pragma once
#include "PUnit.h"

struct list_node_t {
	PUnit* node;
	list_node_t* next;
};

class PUnitList
{
public:
	PUnitList();
	~PUnitList();
	void Append(PUnit* unit);
	list_node_t& Head();
	list_node_t& Tail();
private:
	list_node_t* head;
	list_node_t* tail;
};

