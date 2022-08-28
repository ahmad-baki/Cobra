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

struct ParserReturn {
	Statement* statement;
	Error error;
};


class Parser
{
public:
	Parser(std::string_view text, std::string_view path);
	ParserReturn parse(std::vector<Token> tokens);
	Error parse(std::vector<Token> tokens, BlockNode* block);

	std::string_view text;
	std::string_view path;

private:
	size_t currentPos;
	Token currentToken;
	std::vector<Token> tokenStream;

	void advance();
	void revert(size_t pos);
	ParserReturn getStatement(SymbTable* table);
	ParserReturn getBlockState(SymbTable* table);
	ParserReturn getIfState(SymbTable* table);
	ParserReturn getWhileState(SymbTable* table);
	ParserReturn getDeclState(SymbTable* table);
	ParserReturn getSetState(SymbTable* table);
	ParserReturn getEmptyState(SymbTable* table);

	ParserReturn getPrint(SymbTable* table);

	std::pair<std::vector<ElseCond*>, Error> getIfElse(SymbTable* table);
	ParserReturn getElse(SymbTable* table);

	template<SuppType T>
	std::pair<Expression<T>*, Error> getExpr(SymbTable* table);
	std::pair<IEASTNode*, Error> getIEAST(SymbTable* table);
	std::pair<IEASTNode*, Error> streamToIEAST(std::vector<Token> tokenStream, SymbTable* table);
	std::vector<Token> getExprTokStream();
	std::vector<Token> transExprTokStream(std::vector<Token> tokenStream);
	std::vector<Token> addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op);
};

