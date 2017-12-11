#include "stdafx.h"
#include "PGraphNode.h"


PGraphNode::PGraphNode()
{
}


PGraphNode::~PGraphNode()
{
}

bool PGraphNode::operator==(const PGraphNode & a) const
{
	return index.at(0) == a.index.at(0) && index.at(1) == a.index.at(1) && index.at(2) == a.index.at(2);
}
