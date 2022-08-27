#pragma once
#include "SymbTable.h"
#include "Error.h"


class Statement
{
public:
	SymbTable* table;
	virtual Error run() = 0;
};


