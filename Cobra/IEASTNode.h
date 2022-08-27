#pragma once
#include <string>
#include "SuppType.h"
#include "Expression.h"
#include "Token.h"
#include "SymbTable.h"
#include "Error.h";


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
	
	Error setReturnType(SymbTable* table);

	template<SuppType T>
	std::pair<Expression<T>*, Error> getExpr();

	template<>
	std::pair<Expression<int>*, Error> getExpr();

	template<>
	std::pair<Expression<float>*, Error> getExpr();
};

