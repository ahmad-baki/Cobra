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
	Token token;
	IEASTNode* leftNode{nullptr};
	IEASTNode* rightNode{nullptr};
	std::string_view path;
	std::string_view text;

	
	IEASTNode(std::string_view path, std::string_view text);
	~IEASTNode();
	Expression* getExpr(Error& outError);
};