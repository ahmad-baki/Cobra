#pragma once
#include "IEASTNode.h"
#include <vector>
#include <functional>
#include "BinOp.h"
#include "BlockNode.h"
#include "DeclVar.h"
#include "ElseCond.h"
#include "Error.h"
#include "Expression.h"
#include "GetVar.h"
#include "IfCond.h"
#include "Lexer.h"
#include "Literal.h"
#include "Loop.h"
#include "Statement.h"
#include <string_view>
#include "SetVar.h"
#include "Token.h"
#include "TokenType.h"
#include "SuppType.h"


class Parser
{
public:
	Parser(std::string_view text, std::string_view path);
	std::vector<Statement*> parse(std::vector<Token> tokens, Error& outError);

	std::string_view text;
	std::string_view path;

private:
	size_t currentPos;
	Token currentToken;
	std::vector<Token> tokenStream;

	void advance();
	void revert(size_t pos);
	Statement* getStatement(Error& outError);
	Statement* getBlockState(Error& outError);
	Statement* getIfState(Error& outError);
	Statement* getWhileState(Error& outError);
	Statement* getDeclState(Error& outError);
	Statement* getSetState(Error& outError);
	Statement* getEmptyState(Error& outError);

	Statement* getPrint(Error& outError);

	std::vector<ElseCond*> getIfElse(Error& outError);
	Statement* getElse(Error& outError);

	Expression* getExpr(Error& outError);
	void getIEAST(IEASTNode& rootNode, Error& outError);
	void streamToIEAST(std::vector<Token> tokenStream, IEASTNode& rootNode, Error& outError);
	std::vector<Token> getExprTokStream();
	std::vector<Token> transExprTokStream(std::vector<Token> tokenStream);
	std::vector<Token> addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op);
};

