#pragma once
#include "Expression.h"
#include "SymbTable.h"
#include "SuppType.h"
#include "Statement.h"


class DeclVar : public Statement
{
public:
	DeclVar(std::string name, size_t line, size_t startColumn, size_t endColumn, 
		bool isConst = false, bool isStaticType = false, int typeId = 0, bool isList = false,
		Expression* value = nullptr);
	
	size_t run(Error& outError);
	std::string getName();

private:
	std::string name;
	bool isConst;
	bool isStaticType;
	int typeId;
	Expression* value;
	bool isList;
	//void Init(std::string name, int type, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
};
