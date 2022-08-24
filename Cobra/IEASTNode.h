#pragma once
#include <string>
#include "SuppType.h"
#include "Expression.h"
#include "Token.h"
#include "SymbTable.h"


class IEASTNode
{
public:
	int returnType{ -1 };
	SymbTable* table;
	Token token;
	IEASTNode* leftNode{nullptr};
	IEASTNode* rightNode{nullptr};

	int getReturnType();
	
	IEASTNode(SymbTable* table);
	~IEASTNode();
	
	void setReturnType(SymbTable* table);

	template<SuppType T>
	Expression<T>* getExpr();

	template<>
	Expression<int>* getExpr();

	template<>
	Expression<float>* getExpr();
};

