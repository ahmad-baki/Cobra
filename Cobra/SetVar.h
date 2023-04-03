#pragma once
#include "SymbTable.h"
#include "Expression.h"
#include "SuppType.h"
#include "Statement.h"
#include "Error.h"

class SetVar : public Statement
{
private:
	//SymbTable* table;
	std::string name;
	Expression* index;
	Expression* tValue;

public:

	SetVar(std::string name, Expression* index, Expression* tValue, size_t line, size_t startColumn, size_t endColumn);
	size_t run(Error& outError);
	size_t run(std::vector<Value*> val, Error& outError);
};
