#pragma once
#include <string>
#include "SuppType.h"
#include "Expression.h"
#include "Token.h"
#include "SymbTable.h"


class IEASTNode
{
private:
	int returnType{ -1 };
	SymbTable* table;

public:
	Token* token{nullptr};
	std::string name{ "" };
	IEASTNode* leftNode{nullptr};
	IEASTNode* rightNode{nullptr};

	int getReturnType();
	
	IEASTNode(SymbTable* table);
	
	void setReturnType(SymbTable* table);

	template<SuppType T>
	Expression<T>* getExpr();

	template<>
	Expression<int>* getExpr();

	template<>
	Expression<float>* getExpr();
};

