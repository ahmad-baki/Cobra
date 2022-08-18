#include "BlockNode.h"

// empty code
BlockNode::BlockNode() 
	: statements{std::vector<Statement*>()}, nextExecIndex{0}
{
	table = new SymbTable();
}

BlockNode::BlockNode(std::vector<Statement*> statements, SymbTable* parentTable) :
	statements{statements}, nextExecIndex{ 0 }
{
	table = new SymbTable(parentTable);
}

void BlockNode::run() {
	while (nextExecIndex < statements.size()){
		statements[nextExecIndex]->run();
		nextExecIndex++;
	}
}

void BlockNode::add(Statement* statement)
{
	statements.push_back(statement);
	//statement->table = table;
}
