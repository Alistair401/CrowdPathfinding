#include "stdafx.h"
#include "PUnitList.h"


PUnitList::PUnitList()
{
	head = nullptr;
	tail = nullptr;
}

PUnitList::~PUnitList()
{
	list_node_t* tmp;
	while (head != nullptr) {
		tmp = head;
		head = head->next;
		delete tmp;
	}
}

void PUnitList::Append(PUnit * unit)
{
	list_node_t* new_node = new list_node_t{ unit,nullptr };
	if (head == nullptr) {
		head = new_node;
		tail = head;
	}
	else {
		tail->next = new_node;
	}
}

list_node_t& PUnitList::Head()
{
	return *head;
}

list_node_t& PUnitList::Tail()
{
	return *tail;
}
