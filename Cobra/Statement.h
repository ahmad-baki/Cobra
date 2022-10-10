#pragma once
//#include "SymbTable.h"
#include "Error.h"
#include "Node.h"


class Statement : public Node
{
public:
	//SymbTable* table;
	virtual void run(Error& outError) = 0;
};


