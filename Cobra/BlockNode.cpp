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
		if (outError.errType != ErrorType::NULLERROR)
			return;
		nextExecIndex++;
	}
}

size_t BlockNode::run(Error& outError)
{
	Interpreter::getSingelton()->EnterScope(outError, nullptr, name);
	for (Statement* statement : blockNode) {
		size_t breakCount = statement->run(outError);
		if (outError.errType != ErrorType::NULLERROR)
			return 0;
		if (breakCount > 0) {
			return breakCount;
		}
	}
	Interpreter::getSingelton()->ExitScope();
}


void BlockNode::add(Statement* statement)
{
	blockNode.push_back(statement);
	//statement->table = table;
}
