#pragma once
#include "Expression.h"
#include "SymbTable.h"
#include "SuppType.h"
#include "Statement.h"

template<SuppType T>
class DeclVar : public Statement
{
private:
	std::string name;
	void Init(SymbTable* table);

public:
	DeclVar<T>(std::string name, SymbTable* table);
	void run();
};
