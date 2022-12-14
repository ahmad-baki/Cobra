#pragma once
#include <string>
#include "SuppType.h"
#include "Expression.h"
#include "Token.h"
#include "SymbTable.h"
#include "Error.h"
//#include "Parser.h"

// forward decl.
class Parser;

class IEASTNode
{
public:
	int returnType{ -1 };
	SymbTable* table;
	Token token;
	IEASTNode* leftNode{nullptr};
	IEASTNode* rightNode{nullptr};
	std::string_view path;
	std::string_view text;

	
	IEASTNode(SymbTable* table, std::string_view path, std::string_view text);
	~IEASTNode();

	int getReturnType();

	Error setReturnType(SymbTable* table);

	template<SuppType T>
	std::pair<Expression<T>*, Error> getExpr();

	template<>
	std::pair<Expression<int>*, Error> getExpr();

	template<>
	std::pair<Expression<float>*, Error> getExpr();

private:
};