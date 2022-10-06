#pragma once
#include "Statement.h"
#include "SymbTable.h"
#include <stack>
#include <vector>

class BlockNode : public Statement
{
private:
	//SymbTable table;
	std::vector<Statement*> blockNode;
	size_t nextExecIndex;

public:
	// the empty BlockNode
	BlockNode();
	BlockNode(std::vector<Statement*> blockNode, SymbTable* parentTable);
	void contin(Error& outError);
	void run(Error& outError);
	void add(Statement* statement);
};

