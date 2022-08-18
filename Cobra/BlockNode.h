#pragma once
#include "Statement.h"
#include "SymbTable.h"
#include <stack>
#include <vector>

class BlockNode : public Statement
{
private:
	//SymbTable table;
	std::vector<Statement*> statements;
	size_t nextExecIndex;

public:
	// the empty BlockNode
	BlockNode();
	BlockNode(std::vector<Statement*> statements, SymbTable* parentTable);
	void run();
	void add(Statement* statement);
};

