#pragma once
#include "SymbTable.h"
#include "Expression.h"
#include "SuppType.h"
#include "Statement.h"

template<SuppType T>
class SetVar : public Statement
{
private:
	//SymbTable* table;
	std::string name;
	Expression<T>* value;

public:

	void run();
	SetVar(std::string name, Expression<T>* value, SymbTable* table);
};
