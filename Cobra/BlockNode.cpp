#include "BlockNode.h"

// empty code
BlockNode::BlockNode() 
	: blockNode{std::vector<Statement*>()}, nextExecIndex{0}
{
	table = new SymbTable();
}

BlockNode::BlockNode(std::vector<Statement*> blockNode, SymbTable* parentTable) :
	blockNode{blockNode}, nextExecIndex{ 0 }
{
	table = new SymbTable(parentTable);
}

void BlockNode::contin(Error& outError) {
	while (nextExecIndex < blockNode.size()){
		blockNode[nextExecIndex]->run(outError);
		if (outError.errorName != "NULL")
			return;
		nextExecIndex++;
	}
}

void BlockNode::run(Error& outError)
{
	for (Statement* statement : blockNode) {
		statement->run(outError);
		if (outError.errorName != "NULL")
			return;
	}
	delete table;
}


void BlockNode::add(Statement* statement)
{
	blockNode.push_back(statement);
	//statement->table = table;
}
