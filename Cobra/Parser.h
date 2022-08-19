#pragma once
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
#include "SetVar.h"
#include "SymbTable.h"
#include "Token.h"
#include "TokenType.h"
#include "SuppType.h"
#include "IEASTNode.h"


class Parser
{
private:
	size_t currentPos;
	Token currentToken;
	std::vector<Token> tokenStream;

	void advance();
	void revert(size_t pos);
	Statement* getStatement(SymbTable* table);
	Statement* getStatement1(SymbTable* table);
	Statement* getStatement2(SymbTable* table);
	Statement* getStatement3(SymbTable* table);
	Statement* getStatement5(SymbTable* table);
	Statement* getStatement7(SymbTable* table);
	Statement* getStatement8(SymbTable* table);

	Statement* getPrint(SymbTable* table);

	std::vector<ElseCond*> getIfElse(SymbTable* table);
	Statement* getElse(SymbTable* table);

	template<SuppType T>
	Expression<T>* getExpr(SymbTable* table);
	IEASTNode getIEAST(SymbTable* table);
	IEASTNode streamToIEAST(std::vector<Token> tokenStream, SymbTable* table);
	std::vector<Token> getExprTokStream();
	std::vector<Token> transExprTokStream(std::vector<Token> tokenStream);
	std::vector<Token> addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op);

public:
	Parser();
	BlockNode* parse(std::vector<Token> tokens);
	void parse(std::vector<Token> tokens, BlockNode* block);	
};

