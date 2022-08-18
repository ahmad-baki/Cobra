#pragma once
#include "SymbTable.h"

class Statement
{
public:
	SymbTable* table;
	virtual void run() = 0;
};


