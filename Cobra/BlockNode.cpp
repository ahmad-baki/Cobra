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

Error BlockNode::contin() {
	while (nextExecIndex < statements.size()){
		Error error = statements[nextExecIndex]->run();
		if (error.m_errorName != "NULL")
			return error;
		nextExecIndex++;
	}
	return Error();
}

Error BlockNode::run()
{
	for (Statement* statement : statements) {
		Error error = statement->run();
		if (error.m_errorName != "NULL")
			return error;
	}
	delete table;
	return Error();
}


void BlockNode::add(Statement* statement)
{
	statements.push_back(statement);
	//statement->table = table;
}
