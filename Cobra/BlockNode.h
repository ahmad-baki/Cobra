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
	std::string name;

public:
	// the empty BlockNode
	BlockNode();
	BlockNode(std::vector<Statement*> blockNode, std::string name = "CodeBlock");
	void contin(Error& outError);
	size_t run(Error& outError);
	void add(Statement* statement);
};

