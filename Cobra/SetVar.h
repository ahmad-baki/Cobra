#pragma once
#include "SymbTable.h"
#include "Expression.h"
#include "SuppType.h"
#include "Statement.h"
#include "Error.h"

template<SuppType T>
class SetVar : public Statement
{
private:
	//SymbTable* table;
	std::string name;
	Expression<T>* value;

public:

	Error run();
	SetVar(std::string name, Expression<T>* value, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
};
