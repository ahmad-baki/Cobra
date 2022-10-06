#pragma once
#include "Error.h"
#include <vector>
#include "Statement.h"
#include "BlockNode.h"

class Interpreter
{
public: 
	Interpreter* getSingelton();
	void run(Error& outError);
	void contin(Error& outError);
	void addStatements(std::vector<Statement*> statements);
	void addStatement(Statement* statement);
	void setStatements(BlockNode* blockNode);
private:
	BlockNode* blockNode;
	Interpreter();
	Interpreter* singelton{nullptr};
};

