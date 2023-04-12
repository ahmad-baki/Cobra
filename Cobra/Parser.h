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
#include "GetVal.h"
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
	Parser(/*std::string_view text, std::string_view path*/);
	std::vector<Statement*> parse(std::vector<Token> tokens, Error& outError);

	//std::string_view text;
	//std::string_view path;

private:
	size_t currentPos;
	std::vector<Token> tokenStream;

	void advance();
	void revert(size_t pos);
	Token getCurrToken();

	Statement* getStatement(Error& outError);
	Statement* getBlockState(Error& outError);
	Statement* getIfState(Error& outError);
	Statement* getWhileState(Error& outError);
	Statement* getDeclState(Error& outError);
	Statement* getSetState(Error& outError);
	Statement* getEmptyState(Error& outError);
	Statement* getExprState(Error& outError);
	Statement* getFuncDeclState(Error& outError);
	Statement* getPrint(Error& outError);
	Statement* getReturnState(Error& outError);

	DeclVar* getDeclStateCustSep(Error& outError, std::vector<enum TokenType> sep);


	std::vector<ElseCond*> getIfElse(Error& outError);
	Statement* getElse(Error& outError);

	Expression* getExpr(Error& outError);
	void getIEAST(IEASTNode& rootNode, Error& outError);
	void streamToIEAST(std::vector<Token> tokenStream, IEASTNode& rootNode, Error& outError);
	static std::vector<Token> getExprTokStream(size_t& pos, const std::vector<Token>& tokenStream);
	static std::vector<Token> getArrExprTokStream(size_t& pos, const std::vector<Token>& tokenStream);
	static std::vector<Token> getSingExprTokStream(size_t& pos, const std::vector<Token>& tokenStream);
	static std::vector<Token> getFuncTokStream(const std::vector<Token>& tokenStream, size_t& pos, Error& outError);

	static std::vector<Token> getBraSingExprTokStream(size_t& pos, const std::vector<Token>& tokenStream);


	static std::vector<Token> transExprTokStream(std::vector<Token> tokenStream);
	static std::vector<Token> addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op);

	static size_t findEndOfFunc(std::vector<Token> tokenStream, size_t pos);
	// debug helper
	static int getBrackCount(std::vector<Token> tokStream);
};

