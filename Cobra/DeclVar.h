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
	void Init(std::string name, int type, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);

public:
	DeclVar<T>(std::string name, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
	Error run();
};
