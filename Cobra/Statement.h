#pragma once
//#include "SymbTable.h"
#include "Error.h"
#include "Node.h"


class Statement : public Node
{
public:
	//SymbTable* table;
	virtual size_t run(Error& outError) = 0;
};


