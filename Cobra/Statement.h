#pragma once
#include "SymbTable.h"
#include "Error.h"
#include "Node.h"


class Statement : public Node
{
public:
	SymbTable* table;
	virtual Error run() = 0;
};


