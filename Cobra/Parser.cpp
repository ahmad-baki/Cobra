#include <stack>
#include <algorithm>
#include <stdexcept>
#include "Parser.h"
#include "printState.h"
#include "SyntaxError.h"



Parser::Parser(std::string_view text, std::string_view path)
	: text{ text }, path{ path }, currentPos { 0 }, currentToken{ Token(NONE) }, 
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
	if (currentToken.type == TokenType::NONE) {
		return { nullptr,
			SyntaxError("Reached End of File while Parcing", path, text, currentToken.line,
			currentToken.startColumn, currentToken.endColumn)
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
			return {statement, Error()};

		if (error.m_errorName != "NULL") {
			return { nullptr, error };
		}
	}

	return {nullptr, SyntaxError("input doesnt fit any statement type", path, text, 
		currentToken.line, currentToken.startColumn, currentToken.endColumn) };
}

// Code-Block
ParserReturn Parser::getBlockState(SymbTable* table)
{
	if (currentToken.type != TokenType::LCURLBRACKET)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();
	auto blockNode = new BlockNode(std::vector<Statement*>{}, table);
	while (currentToken.type != TokenType::RCURLBRACKET) {
		auto [statement, error] = getStatement(blockNode->table);
		if (statement == nullptr) {
			revert(startPos);
			return { nullptr, 
				SyntaxError("Reached End of File while Parcing", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn) } ;
		}
		blockNode->add(statement);
	}

	advance();
	blockNode->table->clearReg();
	return { blockNode, Error() };
}

// If-Statement
ParserReturn Parser::getIfState(SymbTable* table)
{
	if (currentToken.type != TokenType::IF)
		return { nullptr, Error() };
	size_t startPos = currentPos;
	advance();

	if (currentToken.type != TokenType::LBRACKET) {
		revert(startPos);
		return { nullptr, Error() };
	}
	advance();

	auto [cond, exprError] = getExpr<bool>(table);
	if (exprError.m_errorName != "NULL") {
		//revert(startPos);
		return { nullptr, exprError };
	}

	if (currentToken.type != TokenType::RBRACKET)
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
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
	if (currentToken.type != TokenType::WHILE)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	if (currentToken.type != TokenType::LBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing '('-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}
	advance();

	auto [cond, exprError] = getExpr<bool>(table);
	if (exprError.m_errorName != "NULL") {
		return { nullptr, exprError };
	}

	if (currentToken.type != TokenType::RBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
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
	if (currentToken.type != TokenType::INTWORD && currentToken.type != TokenType::FLOATWORD) {
		return { nullptr, Error() };
	}
	auto returnType = currentToken.type;

	size_t startPos = currentPos;
	advance();

	if (currentToken.type != TokenType::IDENTIFIER) {
		return { nullptr,
			SyntaxError("No variablename found", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}
	std::string varName = currentToken.value;

	Statement* decl;
	if (returnType == TokenType::INTWORD) {
		decl = new DeclVar<int>(varName, table);
		advance();
	}
	else if (returnType == TokenType::FLOATWORD) {
		decl = new DeclVar<float>(varName, table);
		advance();
	}
	else {
		return { nullptr,
			SyntaxError("Invalid Datatype", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}

	// only decl
	if (currentToken.type == TokenType::SEMICOLON) {
		advance();
		return { decl, Error() };
	}
	// decl + init
	if (currentToken.type == TokenType::EQ) {
		// not very clean, but does it job
		revert(currentPos - 1);

		auto [setVar, error] = getSetState(table);

		if (error.m_errorName != "NULL") {
			return { nullptr, error };
		}

		return { new BlockNode(std::vector<Statement*>{
			decl, setVar
		}, nullptr), Error() };
	}
	return { nullptr,
		SyntaxError("Invalid Token", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
	};
}

// setVar
ParserReturn Parser::getSetState(SymbTable* table)
{
	if (currentToken.type != TokenType::IDENTIFIER) {
		return { nullptr, Error() };
	}
	std::string varName = currentToken.value;

	size_t startPos = currentPos;
	advance();

	if (currentToken.type != TokenType::EQ) {
		revert(startPos);
		return { nullptr, Error() };
	}
	advance();
	// get expr
	int type = table->varReg[varName];
	if (type == 0){
		auto [expr, error] = getExpr<int>(table);

		if (error.m_errorName != "NULL")
			return { nullptr, error };

		SetVar<int>* setVar = new SetVar<int>(varName, expr, table);

		if (currentToken.type != TokenType::SEMICOLON) {
			delete setVar;
			return { nullptr,
				SyntaxError("Presumably Missing ';'", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
			};
		}
		advance();
		return { setVar, Error()};
	}
	if (type == 1) {
		auto [expr, error] = getExpr<float>(table);

		if (error.m_errorName != "NULL") 
			return { nullptr, error };

		SetVar<float>* setVar = new SetVar<float>(varName, expr, table);

		if (currentToken.type != TokenType::SEMICOLON) {
			delete setVar;
			return { nullptr,
				SyntaxError("Presumably Missing ';'", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
			};
		}
		advance();
		return { setVar, Error() };
	}
	return { nullptr,
		SyntaxError("variable " + varName + " has invalid type", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
	};
}

// Empty statement
ParserReturn Parser::getEmptyState(SymbTable* table)
{
	if (currentToken.type != TokenType::SEMICOLON)
		return { nullptr, Error() };
	advance();
	return { new BlockNode(), Error() };
}

// Print statement
ParserReturn Parser::getPrint(SymbTable* table) 
{
	if (currentToken.type != TokenType::IDENTIFIER || 
		currentToken.value != "print")
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	if (currentToken.type != TokenType::LBRACKET) {
		return { nullptr,
			SyntaxError("Presumably Missing '('-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}
	advance();
	auto [rootNode, error] = getIEAST(table);

	if (error.m_errorName != "NULL")
		return { nullptr, error };

	if (currentToken.type != TokenType::RBRACKET) {
		delete rootNode;		
		return { nullptr,
			SyntaxError("Presumably Missing ')'-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}
	advance();
	if (currentToken.type != TokenType::SEMICOLON) {
		delete rootNode;
		return { nullptr,
			SyntaxError("Presumably Missing ';'", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	}
	advance();

	// int
	if (rootNode->getReturnType() == 0) {
		auto [expr, exprError] = rootNode->getExpr<int>();

		if (exprError.m_errorName != "NULL")
			return { nullptr, exprError };

		return { new PrintState(expr), Error() };
	}

	// float
	if (rootNode->getReturnType() == 1) {
		auto [expr, exprError] = rootNode->getExpr<float>();

		if (exprError.m_errorName != "NULL")
			return { nullptr, exprError };

		return { new PrintState(expr), Error() };
	}

	// its constructed on the heap
	delete rootNode;
	return { nullptr,
			SyntaxError("Invalid type of expression", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
	};
}


std::pair<std::vector<ElseCond*>, Error> Parser::getIfElse(SymbTable* table)
{
	auto output = std::vector<ElseCond*>();
	while (currentToken.type == TokenType::ELSE) {
		size_t startPos = currentPos;
		advance();

		if (currentToken.type != TokenType::IF)
		{
			revert(startPos);
			break;
		}

		advance();

		if (currentToken.type != TokenType::LBRACKET) {
			return { {},
				SyntaxError("Presumably Missing '('-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
			};
		}
		advance();

		auto [cond, exprError] = getExpr<bool>(table);
		if (exprError.m_errorName != "NULL") {
			return { {}, exprError };
		}
		if (currentToken.type != TokenType::RBRACKET) {
			return { {},
				SyntaxError("Presumably Missing ')'-Bracket", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
			};
		}

		advance();

		auto [statement, stateError] = getStatement(table);
		if (stateError.m_errorName != "NULL") {
			return { {}, exprError };
		}
		output.push_back(new ElseCond(cond, statement));
	}
	return { output, Error() };
}


ParserReturn Parser::getElse(SymbTable* table) {
	if (currentToken.type != TokenType::ELSE)
		return { nullptr, Error() };

	size_t startPos = currentPos;
	advance();

	auto [statement, error] = getStatement(table);
	if (error.m_errorName != "NULL") {
		return { nullptr, error };
	}
	return { statement, Error()};
}


template<SuppType T>
inline std::pair<Expression<T>*, Error> Parser::getExpr(SymbTable* table)
{
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return { nullptr, SyntaxError("Invalid Expression", path, text, 
			currentToken.line, currentToken.startColumn, currentToken.endColumn) };

	tokenStream = transExprTokStream(tokenStream);
	auto [rootNode, exprError] = streamToIEAST(tokenStream, table);

	if (exprError.m_errorName != "NULL")
		return { nullptr, SyntaxError("Invalid Expression", path, text,
			currentToken.line, currentToken.startColumn, currentToken.endColumn) };

	auto [expr, error] = rootNode->getExpr<T>();
	delete rootNode;
	return { expr, error };
}

// delete the node, becouse its constructed on the heap
std::pair<IEASTNode*, Error> Parser::getIEAST(SymbTable* table) {
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return { nullptr, SyntaxError("Invalid Expression", path, text,
			currentToken.line, currentToken.startColumn, currentToken.endColumn) };

	tokenStream = transExprTokStream(tokenStream);
	auto [rootNode, error] = streamToIEAST(tokenStream, table);
	return { rootNode, error };
}

// delete the node, becouse its constructed on the heap
std::pair<IEASTNode*, Error> Parser::streamToIEAST(std::vector<Token> tokenStream, SymbTable* table)
{
	IEASTNode* rootNode = new IEASTNode(table);
	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(rootNode);
	for (size_t i = 0; i < tokenStream.size(); i++) {
		switch (tokenStream[i].type)
		{
		case TokenType::LBRACKET:
		{
			IEASTNode* lNode = new IEASTNode(table);
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
			IEASTNode* rightNode = new IEASTNode(table);
			topNode->rightNode = rightNode;
			nodeStack.push(rightNode);
			break;
		}
	}
	if (nodeStack.size() > 0)
		return { nullptr,
			SyntaxError("IEASTNode - Stack has elements over", path, text, currentToken.line, currentToken.startColumn, currentToken.endColumn)
		};
	rootNode->setReturnType(table);
	return { rootNode, Error() };
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream() {
	size_t startPos = currentPos;
	size_t bracketSur = 0;
	bool mustValue = true;
	TokenType returnType = currentToken.type;

	std::vector<enum TokenType> valTypes{
		TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER
	};
	std::vector<enum TokenType> binOps{
		TokenType::PLUS, TokenType::MINUS, TokenType::MUL,
		TokenType::DIV, TokenType::EQEQ, TokenType::EXCLAEQ
	};

	while (returnType != TokenType::SEMICOLON &&
		!(returnType == TokenType::RBRACKET && bracketSur == 0)
		&& currentToken.type != TokenType::NONE) 
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
		returnType = currentToken.type;
	}
	if (bracketSur > 0)
		return std::vector<Token>();

	return std::vector<Token>(tokenStream.begin() + startPos, tokenStream.begin() + currentPos);
}

std::vector<Token> Parser::transExprTokStream(std::vector<Token> tokenStream) {
	// order decides op-binding-strength: strong -> weak 
	std::vector<enum TokenType> opClasses[]{
		{TokenType::MUL, TokenType::DIV},
		{TokenType::PLUS, TokenType::MINUS},
		{TokenType::EQEQ, TokenType::EXCLAEQ},
	};

	for (auto opClass : opClasses)
		tokenStream = addBrackets(tokenStream, opClass);
	return tokenStream;
}

std::vector<Token> Parser::addBrackets(std::vector<Token> tokenStream, std::vector<enum TokenType> op)
{
	auto valExpr = { TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER };
	for (int i = 1; i < tokenStream.size(); i++) {
		// if token is a strong-binding operator
		if (std::find(op.begin(), op.end(), tokenStream[i].type) != op.end()) {

			size_t bracketSur = 0;
			// places a LBracket
			for (int j = i - 1; j > -1; j--) {
				if (j == 0) {
					tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
					// i know its useless, but it looks good
					break;
				}
				else if (tokenStream[j].type == TokenType::LBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
						bracketSur++;
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j].type == TokenType::RBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].type) != valExpr.end()
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
				else if (tokenStream[j].type == TokenType::RBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j].type == TokenType::LBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].type) != valExpr.end()
					&& bracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j+1, Token(TokenType::RBRACKET));
					break;
				}
			}
		}
	}
	return tokenStream;
}
