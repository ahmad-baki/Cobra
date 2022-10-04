#include <stack>
#include <algorithm>
#include <stdexcept>
#include "Parser.h"
#include "printState.h"
#include "SyntaxError.h"



Parser::Parser(std::string_view text, std::string_view path)
	: text{ text }, path{ path }, currentPos { 0 }, currentToken{ Token(TokenType::NONE) }, 
	tokenStream{ std::vector<Token>() }
{
}

ParserReturn Parser::parse(std::vector<Token> tokens)
{
	BlockNode* block = new BlockNode();
	Error error = parse(tokens, block);
	return { block, error };
}

Error Parser::parse(std::vector<Token> tokens, BlockNode* block)
{
	tokenStream = tokens;
	currentToken = tokenStream[0];
	revert(0);

	std::vector<Statement*> statements{};
	// main-parser loop
	while (currentPos < tokens.size()) {
		auto [statement, error] = getStatement(block->table);
		if (statement == nullptr) {
			return error;
		}
		statements.push_back(statement);
	}
	for (Statement* statement : statements) {
		block->add(statement);
	}
	return Error();
}

void Parser::advance()
{
	currentPos++;
	if (currentPos > tokenStream.size() - 1){
		Token newToken = Token(TokenType::NONE, currentToken.line, currentToken.endColumn + 1, currentToken.endColumn + 2);
		currentToken = newToken;
	}
	else
		currentToken = tokenStream[currentPos];
}

void Parser::revert(size_t pos) {
	currentPos = pos;
	if (currentPos > tokenStream.size())
		currentToken = Token(TokenType::NONE);
	else
		currentToken = tokenStream[currentPos];
}

ParserReturn Parser::getStatement(SymbTable* table)
{
	if (currentToken.dataType == TokenType::NONE) {
		return { nullptr,
			SyntaxError("Reached End of File while Parcing", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}

	ParserReturn(Parser:: *statementTypes[])(SymbTable * table) = {
		&Parser::getBlockState, 
		&Parser::getIfState,
		&Parser::getWhileState,
		&Parser::getDeclState,
		&Parser::getSetState,
		&Parser::getEmptyState,
		&Parser::getPrint
	};

	for (auto statementType : statementTypes) {
		auto [statement, error] = (this->*statementType)(table);
		if (statement != nullptr)
			return { statement, Error() };

		if (error.m_errorName != "NULL") {
			return { nullptr, error };
		}
	}
	return {nullptr, SyntaxError("input doesnt fit any statement type", currentToken.line,
		currentToken.startColumn, currentToken.endColumn, path, text) };
}

// Code-Block
ParserReturn Parser::getBlockState(SymbTable* table)
{
	if (currentToken.dataType != TokenType::LCURLBRACKET)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();
	auto blockNode = new BlockNode(std::vector<Statement*>{}, table);
	while (currentToken.dataType != TokenType::RCURLBRACKET) {
		auto [statement, error] = getStatement(blockNode->table);

		if (error.m_errorName != "NULL") {
			return { nullptr, error } ;
		}

		blockNode->add(statement);
	}

	advance();
	return { blockNode, Error() };
}

// If-Statement
ParserReturn Parser::getIfState(SymbTable* table)
{
	if (currentToken.dataType != TokenType::IF)
		return { nullptr, Error() };
	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		revert(startPos);
		return { nullptr, Error() };
	}
	advance();

	auto [cond, exprError] = getExpr(table);
	if (exprError.m_errorName != "NULL") {
		//revert(startPos);
		return { nullptr, exprError };
	}

	if (currentToken.dataType != TokenType::RBRACKET)
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};

	advance();

	auto [statement, stateError] = getStatement(table);
	if (stateError.m_errorName != "NULL")
		return { nullptr, stateError };

	auto [ifElseStates, ifElseError] = getIfElse(table);

	if (ifElseError.m_errorName != "NULL")
		return { nullptr, ifElseError };

	auto [elseState, elseError] = getElse(table);

	if (elseError.m_errorName != "NULL")
		return { nullptr, elseError };

	return { new IfCond(cond, statement, ifElseStates, elseState), Error()};
}

// While-Loop
ParserReturn Parser::getWhileState(SymbTable* table)
{
	if (currentToken.dataType != TokenType::WHILE)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing '('-Bracket", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	advance();

	auto [cond, exprError] = getExpr(table);
	if (exprError.m_errorName != "NULL") {
		return { nullptr, exprError };
	}

	if (currentToken.dataType != TokenType::RBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}

	advance();

	auto [statement, stateError] = getStatement(table);

	if (stateError.m_errorName != "NULL") {
		return { nullptr, stateError };
	}

	return { new Loop(cond, statement), Error() };
}

// Decl (initialization)
ParserReturn Parser::getDeclState(SymbTable* table)
{
	bool constVar{false};
	size_t startPos = currentPos;

	if (currentToken.dataType == TokenType::CONSTWORD) {
		constVar = true;
		advance();
	}
	if (currentToken.dataType != TokenType::INTWORD && 
		currentToken.dataType != TokenType::FLOATWORD &&
		currentToken.dataType != TokenType::VARWORD)
	{
		if (constVar) {
			revert(startPos);
			return { nullptr,
				SyntaxError("No type-specifier found", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text)
			};
		}
		return { nullptr, Error() };
	}
	auto returnType = currentToken.dataType;
	advance();

	if (currentToken.dataType != TokenType::IDENTIFIER) {
		return { nullptr,
			SyntaxError("No variablename found", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	std::string varName = currentToken.value;

	Value::DataType dataType;
	switch (returnType)
	{
	case TokenType::INTWORD:
		dataType = Value::INTTYPE;
		break;
	case TokenType::FLOATWORD:
		dataType = Value::DECTYPE;
		break;
	case TokenType::VARWORD:
		dataType = Value::UNDEFINED;
		break;
	default:
		revert(startPos);
		return { nullptr,
			SyntaxError("Invalid Datatype", currentToken.line,
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}

	advance();
	Expression* expr{ nullptr };
	// only decl
	if (currentToken.dataType == TokenType::SEMICOLON) {
		advance();
	}
	// decl + init
	else if (currentToken.dataType == TokenType::EQ) {
		advance();
		auto exprReturn = getExpr(table);

		if (exprReturn.second.m_errorName != "NULL") {
			revert(startPos);
			return { nullptr, exprReturn.second };
		}
		expr = exprReturn.first;
	}
	else {
		revert(startPos);
		return { nullptr,
			SyntaxError("Invalid Token", currentToken.line,
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	Statement* decl = new DeclVar(varName, table, currentToken.line, tokenStream[startPos].startColumn, currentToken.endColumn, constVar, true, dataType, expr);
	return { decl, Error() };
}

// setVar
ParserReturn Parser::getSetState(SymbTable* table)
{
	if (currentToken.dataType != TokenType::IDENTIFIER) {
		return { nullptr, Error() };
	}
	std::string varName = currentToken.value;

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::EQ) {
		revert(startPos);
		return { nullptr, Error() };
	}
	advance();
	// get expr
	auto [expr, error] = getExpr(table);

	if (error.m_errorName != "NULL")
		return { nullptr, error };

	SetVar* setVar = new SetVar(varName, expr, table, currentToken.line, 
		tokenStream[startPos].startColumn, currentToken.endColumn);

	if (currentToken.dataType != TokenType::SEMICOLON) {
		delete setVar;
		return { nullptr,
			SyntaxError("Presumably Missing ';'", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	advance();
	return { setVar, Error()};
}

// Empty statement
ParserReturn Parser::getEmptyState(SymbTable* table)
{
	if (currentToken.dataType != TokenType::SEMICOLON)
		return { nullptr, Error() };
	advance();
	return { new BlockNode(), Error() };
}

// Print statement
ParserReturn Parser::getPrint(SymbTable* table) 
{
	if (currentToken.dataType != TokenType::IDENTIFIER || 
		currentToken.value != "print")
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing '('-Bracket", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	advance();
	auto [expr, exprError] = getExpr(table);

	if (exprError.m_errorName != "NULL")
		return { nullptr, exprError };

	if (currentToken.dataType != TokenType::RBRACKET) {
		delete expr;
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	advance();
	if (currentToken.dataType != TokenType::SEMICOLON) {
		delete expr;
		return { nullptr,
			SyntaxError("Presumably Missing ';'", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	}
	advance();
	return { new PrintState(expr), Error() };
}

// list of else if statements
std::pair<std::vector<ElseCond*>, Error> Parser::getIfElse(SymbTable* table)
{
	auto output = std::vector<ElseCond*>();
	while (currentToken.dataType == TokenType::ELSE) {
		size_t startPos = currentPos;
		advance();

		if (currentToken.dataType != TokenType::IF)
		{
			revert(startPos);
			break;
		}

		advance();

		if (currentToken.dataType != TokenType::LBRACKET) {
			return { {},
				SyntaxError("Presumably Missing '('-Bracket", currentToken.line, currentToken.startColumn, currentToken.endColumn, path, text)
			};
		}
		advance();

		auto [cond, exprError] = getExpr(table);
		if (exprError.m_errorName != "NULL") {
			return { {}, exprError };
		}
		if (currentToken.dataType != TokenType::RBRACKET) {
			return { {},
				SyntaxError("Presumably Missing ')'-Bracket", currentToken.line, currentToken.startColumn, currentToken.endColumn, path, text)
			};
		}

		advance();

		auto [statement, stateError] = getStatement(table);
		if (stateError.m_errorName != "NULL") {
			return { {}, exprError };
		}
		output.push_back(new ElseCond(cond, statement, currentToken.line, tokenStream[startPos].startColumn, currentToken.endColumn));
	}
	return { output, Error() };
}

// else statement
ParserReturn Parser::getElse(SymbTable* table) {
	if (currentToken.dataType != TokenType::ELSE)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	auto [statement, error] = getStatement(table);
	if (error.m_errorName != "NULL") {
		return { nullptr, error };
	}
	return { statement, Error()};
}

std::pair<Expression*, Error> Parser::getExpr(SymbTable* table)
{
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return { nullptr, SyntaxError("Invalid Expression",	currentToken.line, 
			currentToken.startColumn, currentToken.endColumn, path, text) };

	tokenStream = transExprTokStream(tokenStream);
	auto [rootNode, exprError] = streamToIEAST(tokenStream, table);

	if (exprError.m_errorName != "NULL")
		return { nullptr, SyntaxError("Invalid Expression", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text) };

	auto [expr, error] = rootNode->getExpr();
	delete rootNode;
	return { expr, error };
}

// delete the node, becouse its constructed on the heap
std::pair<IEASTNode*, Error> Parser::getIEAST(SymbTable* table) {
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return { nullptr, SyntaxError("Invalid Expression", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text) };

	tokenStream = transExprTokStream(tokenStream);
	auto [rootNode, error] = streamToIEAST(tokenStream, table);
	return { rootNode, error };
}

// delete the node, becouse its constructed on the heap
std::pair<IEASTNode*, Error> Parser::streamToIEAST(std::vector<Token> tokenStream, SymbTable* table)
{
	IEASTNode* rootNode = new IEASTNode(table, path, text);
	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(rootNode);
	for (size_t i = 0; i < tokenStream.size(); i++) {
		switch (tokenStream[i].dataType)
		{
		case TokenType::LBRACKET:
		{
			IEASTNode* lNode = new IEASTNode(table, path, text);
			nodeStack.top()->leftNode = lNode;
			nodeStack.push(lNode);
			break;
		}
		case TokenType::DECLIT:
		{
			IEASTNode* DecNode = nodeStack.top();
			nodeStack.pop();
			DecNode->token = tokenStream[i];
			break;
		}
		case TokenType::INTLIT:
		{
			IEASTNode* IntNode = nodeStack.top();
			nodeStack.pop();
			IntNode->token = tokenStream[i];
			break;
		}
		case TokenType::IDENTIFIER:
		{
			IEASTNode* varNode = nodeStack.top();
			nodeStack.pop();
			varNode->token = tokenStream[i];
			break;
		}
		case TokenType::RBRACKET:
		{
			nodeStack.pop();
			break;
		}

		// operator
		default:
			IEASTNode* topNode = nodeStack.top();
			topNode->token = tokenStream[i];
			IEASTNode* rightNode = new IEASTNode(table, path, text);
			topNode->rightNode = rightNode;
			nodeStack.push(rightNode);
			break;
		}
	}
	if (nodeStack.size() > 0)
		return { nullptr,
			SyntaxError("IEASTNode - Stack has elements over", currentToken.line, 
				currentToken.startColumn, currentToken.endColumn, path, text)
		};
	return { rootNode, Error() };
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream() {
	size_t startPos = currentPos;
	size_t bracketSur = 0;
	bool mustValue = true;
	TokenType returnType = currentToken.dataType;

	std::vector<enum TokenType> valTypes{
		TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER
	};
	std::vector<enum TokenType> binOps{
		TokenType::PLUS, TokenType::MINUS, TokenType::MUL,
		TokenType::DIV, TokenType::EQEQ, TokenType::EXCLAEQ,
		TokenType::BIG, TokenType::BIGEQ, TokenType::SMALL, 
		TokenType::SMALLEQ, TokenType::AND, TokenType::OR,
		TokenType::MOD,
	};

	while (returnType != TokenType::SEMICOLON || returnType != TokenType::COMMA &&
		!(returnType == TokenType::RBRACKET && bracketSur == 0)
		&& currentToken.dataType != TokenType::NONE) 
	{

		if (mustValue) {
			if (returnType != TokenType::LBRACKET) {
				if (std::find(valTypes.begin(), valTypes.end(), returnType) == valTypes.end()) {
					revert(startPos);
					return std::vector<Token>();
				}
				mustValue = false;
			}
			else {
				bracketSur++;
			}
		}
		else {
			if (returnType != TokenType::RBRACKET) {
				if (std::find(binOps.begin(), binOps.end(), returnType) == binOps.end()) {
					revert(startPos);
					return std::vector<Token>();
				}
				mustValue = true;
			}
			else {
				bracketSur--;
			}
		}
		advance();
		returnType = currentToken.dataType;
	}
	if (bracketSur > 0)
		return std::vector<Token>();

	return std::vector<Token>(tokenStream.begin() + startPos, tokenStream.begin() + currentPos);
}

// adds Brackets around operators according to their priorities
std::vector<Token> Parser::transExprTokStream(std::vector<Token> tokenStream) {
	// order decides op-binding-strength: strong -> weak 
	std::vector<enum TokenType> opClasses[]{
		{TokenType::MUL, TokenType::DIV, TokenType::MOD},
		{TokenType::PLUS, TokenType::MINUS},
		{TokenType::EQEQ, TokenType::EXCLAEQ, TokenType::SMALL, TokenType::SMALLEQ, TokenType::BIG, TokenType::BIGEQ, },
		{TokenType::AND, TokenType::OR},
	};

	for (auto opClass : opClasses)
		tokenStream = addBrackets(tokenStream, opClass);
	return tokenStream;
}

// add brackets around given operators
std::vector<Token> Parser::addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op)
{
	auto valExpr = { TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER };
	for (int i = 1; i < tokenStream.size(); i++) {
		// if token is a strong-binding operator
		if (std::find(op.begin(), op.end(), tokenStream[i].dataType) != op.end()) {

			size_t bracketSur = 0;
			// places a LBracket
			for (int j = i - 1; j > -1; j--) {
				if (j == 0) {
					tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
					// i know its useless, but it looks good
					break;
				}
				else if (tokenStream[j].dataType == TokenType::LBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
						bracketSur++;
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j].dataType == TokenType::RBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].dataType) != valExpr.end()
					&& bracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
					bracketSur++;
					break;
				}
			}
			// becouse there is now a lBracket left from i
			i++;

			bracketSur = 0;
			// places a RBracket
			for (int j = i + 1; j < tokenStream.size(); j++) {
				if (j == tokenStream.size() - 1) {
					tokenStream.insert(tokenStream.end(), Token(TokenType::RBRACKET));
					break;
				}
				else if (tokenStream[j].dataType == TokenType::RBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j].dataType == TokenType::LBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].dataType) != valExpr.end()
					&& bracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j+1, Token(TokenType::RBRACKET));
					break;
				}
			}
		}
	}
	return tokenStream;
}
