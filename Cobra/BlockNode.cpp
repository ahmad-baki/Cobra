#include "BlockNode.h"
#include "Interpreter.h"

// empty code
BlockNode::BlockNode() 
	: blockNode{std::vector<Statement*>()}, nextExecIndex{0}
{
	//table = new SymbTable();
}

BlockNode::BlockNode(std::vector<Statement*> blockNode, std::string name) :
	blockNode{blockNode}, name{ name }, nextExecIndex{0}
{
	//table = new SymbTable(parentTable);
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
	Interpreter::getSingelton()->EnterScope(outError, nullptr, name);
	for (Statement* statement : blockNode) {
		statement->run(outError);
		if (outError.errorName != "NULL")
			return;
	}
	Interpreter::getSingelton()->ExitScope();
}


void BlockNode::add(Statement* statement)
{
	blockNode.push_back(statement);
	//statement->table = table;
}
