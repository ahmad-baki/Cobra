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
	Expression* value;

public:

	void run(Error& outError);
	SetVar(std::string name, Expression* value, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
};
