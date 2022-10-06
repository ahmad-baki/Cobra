#pragma once
#include "Expression.h"
#include "SymbTable.h"
#include "SuppType.h"
#include "Statement.h"


class DeclVar : public Statement
{
public:
	DeclVar(std::string name, SymbTable* table, size_t line, size_t startColumn, 
		size_t endColumn, bool isConst = false, bool isStaticType = false, Value::DataType dataType = Value::UNDEFINED, Expression* expr = nullptr);
	void run(Error& outError);

private:
	std::string name;
	bool isConst;
	bool isStaticType;
	Value::DataType dataType;
	Expression* expr;
	//void Init(std::string name, int type, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
};
