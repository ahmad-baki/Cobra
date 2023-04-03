#pragma once
#include <string>
#include <filesystem>
#include "SuppType.h"
#include "Expression.h"
#include "Token.h"
#include "SymbTable.h"
#include "Error.h"
//#include "Parser.h"

namespace fs = std::filesystem;

// forward decl.
class Parser;

class IEASTNode
{
public:
	Token token;
	IEASTNode* leftNode{nullptr};
	IEASTNode* rightNode{nullptr};
	fs::path path;
	std::string_view text;

	
	IEASTNode(fs::path path, std::string_view text);
	~IEASTNode();
	virtual Expression* getExpr(Error& outError);
};