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
#include "SymbTable.h"
#include "Token.h"
#include "TokenType.h"
#include "SuppType.h"


class Parser
{
public:
	Parser(std::string_view text, std::string_view path);
	Statement* parse(std::vector<Token> tokens, Error& outError);
	void parse(std::vector<Token> tokens, BlockNode* block, Error& outError);

	std::string_view text;
	std::string_view path;

private:
	size_t currentPos;
	Token currentToken;
	std::vector<Token> tokenStream;

	void advance();
	void revert(size_t pos);
	Statement* getStatement(SymbTable* table, Error& outError);
	Statement* getBlockState(SymbTable* table, Error& outError);
	Statement* getIfState(SymbTable* table, Error& outError);
	Statement* getWhileState(SymbTable* table, Error& outError);
	Statement* getDeclState(SymbTable* table, Error& outError);
	Statement* getSetState(SymbTable* table, Error& outError);
	Statement* getEmptyState(SymbTable* table, Error& outError);

	Statement* getPrint(SymbTable* table, Error& outError);

	std::vector<ElseCond*> getIfElse(SymbTable* table, Error& outError);
	Statement* getElse(SymbTable* table, Error& outError);

	Expression* getExpr(SymbTable* table, Error& outError);
	void getIEAST(SymbTable* table, IEASTNode& rootNode, Error& outError);
	void streamToIEAST(std::vector<Token> tokenStream, SymbTable* table, IEASTNode& rootNode, Error& outError);
	std::vector<Token> getExprTokStream();
	std::vector<Token> transExprTokStream(std::vector<Token> tokenStream);
	std::vector<Token> addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op);
};

