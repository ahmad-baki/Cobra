#include <stack>
#include <algorithm>
#include <stdexcept>
#include "Parser.h"
#include "IEASTNode.h"



Parser::Parser()
	: currentPos{0}, currentToken{Token(NONE)}, tokenStream{std::vector<Token>()}
{
}

BlockNode* Parser::parse(std::vector<Token> tokens)
{
	BlockNode* block = new BlockNode();
	parse(tokens, block);
	return block;
}

void Parser::parse(std::vector<Token> tokens, BlockNode* block)
{
	tokenStream = tokens;
	currentToken = tokenStream[0];
	revert(0);
	Statement* statement = getStatement(block->table);
	// main-parser loop
	while (statement != nullptr) {
		block->add(statement);
		statement = getStatement(block->table);
	}
	block->table->clearReg();

	// add check wether EOF-Token is reached
}

void Parser::advance() 
{
	currentPos++;
	if (currentPos > tokenStream.size()-1)
		currentToken = Token(TokenType::NONE);
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

Statement* Parser::getStatement(SymbTable* table)
{
	Statement* (Parser:: *statementTypes[])(SymbTable * table) = {
		&Parser::getStatement1, 
		&Parser::getStatement2,
		&Parser::getStatement3,
		&Parser::getStatement5,
		&Parser::getStatement7,
		&Parser::getStatement8
	};

	for (auto statementType : statementTypes) {
		Statement* statement = (this->*statementType)(table);
		if (statement != nullptr)
			return statement;
	}

	return nullptr;
}

// Code-Block
Statement* Parser::getStatement1(SymbTable* table)
{
	if (currentToken._type != TokenType::LCURLBRACKET)
		return nullptr;
	size_t startPos = currentPos;
	advance();
	auto blockNode = new BlockNode(std::vector<Statement*>{}, table);
	while (currentToken._type != TokenType::RCURLBRACKET) {
		Statement* statement = getStatement(blockNode->table);
		if (statement == nullptr) {
			revert(startPos);
			return nullptr;
		}
		(*blockNode).add(statement);
		advance();
	}
	blockNode->table->clearReg();
	return blockNode;
}

// If-Statement
Statement* Parser::getStatement2(SymbTable* table)
{
	if (currentToken._type != TokenType::IF)
		return nullptr;
	size_t startPos = currentPos;
	advance();
	Expression<bool>* cond = getExpr<bool>(table);
	if (cond == nullptr) {
		revert(startPos);
		return nullptr;
	}

	Statement* statement = getStatement(table);
	if (statement == nullptr) {
		revert(startPos);
		return nullptr;
	}

	std::vector<ElseCond*> ifElseStates = getIfElse(table);
	Statement* elseState = getElse(table);
	return new IfCond(cond, statement, ifElseStates, elseState);
}

// While-Loop
Statement* Parser::getStatement3(SymbTable* table)
{
	if (currentToken._type != TokenType::WHILE)
		return nullptr;

	size_t startPos = currentPos;
	advance();
	Expression<bool>* cond = getExpr<bool>(table);

	if (cond != nullptr) {
		revert(startPos);
		return nullptr;
	}

	Statement* statement = getStatement(table);
	if (statement == nullptr) {
		revert(startPos);
		return nullptr;
	}

	return new Loop(cond, statement);
}

// Decl (initialization)
Statement* Parser::getStatement5(SymbTable* table)
{
	if (currentToken._type != TokenType::IDENTIFIER) {
		return nullptr;
	}
	std::string returnType = currentToken._value;

	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::IDENTIFIER) {
		revert(startPos);
		return nullptr;
	}
	std::string varName = currentToken._value;


	if (returnType == "int") {
		DeclVar<int>* decl = new DeclVar<int>(varName, table);
		advance();

		// only decl
		if (currentToken._type == TokenType::SEMICOLON) {
			advance();
			return decl;
		}
		// decl + init
		if (currentToken._type == TokenType::EQ) {
			// not very clean, but does it job
			revert(currentPos - 1);
			Statement* setVar = getStatement7(table);
			return new BlockNode(std::vector<Statement*>{
				decl, setVar
			}, nullptr);
		}
	}
	else if (returnType == "float") {
		DeclVar<float>* decl = new DeclVar<float>(varName, table);
		advance();

		// only decl
		if (currentToken._type == TokenType::SEMICOLON) {
			advance();
			return decl;
		}
		// decl + init
		if (currentToken._type == TokenType::EQ) {
			// not very clean, but does it job
			revert(currentPos - 1);
			Statement* setVar = getStatement7(table);
			return new BlockNode(std::vector<Statement*>{
				decl, setVar
			}, nullptr);
		}
	}

	revert(startPos);
	return nullptr;
}

// setVar
Statement* Parser::getStatement7(SymbTable* table)
{
	if (currentToken._type != TokenType::IDENTIFIER) {
		return nullptr;
	}
	std::string varName = currentToken._value;

	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::EQ) {
		revert(startPos);
		return nullptr;
	}
	advance();
	// get expr
	int type = table->varReg[varName];
	if (type == 0){
		Expression<int>* expr = getExpr<int>(table);
		SetVar<int>* setVar = new SetVar<int>(varName, expr, table);
		if (currentToken._type != TokenType::SEMICOLON) {
			delete setVar;
				revert(startPos);
				return nullptr;
		}
		advance();
		return setVar;
	}
	if (type == 1) {
		Expression<float>* expr = getExpr<float>(table);
		SetVar<float>* setVar = new SetVar<float>(varName, expr, table);
		if (currentToken._type != TokenType::SEMICOLON) {
			delete setVar;
			revert(startPos);
			return nullptr;
		}
		advance();
		return setVar;
	}
	else {
		throw std::invalid_argument("variable " + varName + " has invalid argument");
	}
	revert(startPos);
	return nullptr;
}

// Empty statement
Statement* Parser::getStatement8(SymbTable* table)
{
	if (currentToken._type != TokenType::SEMICOLON)
		return nullptr;
	advance();
	return new BlockNode();
}

std::vector<ElseCond*> Parser::getIfElse(SymbTable* table)
{
	auto output = std::vector<ElseCond*>();
	while (currentToken._type == TokenType::ELSE) {
		size_t startPos = currentPos;
		advance();

		if (currentToken._type != TokenType::IF) {
			revert(startPos);
			break;
		}

		advance();
		Expression<bool>* cond = getExpr<bool>(table);
		if (cond == nullptr) {
			revert(startPos);
			break;
		}

		Statement* statement = getStatement(table);
		if (statement == nullptr) {
			revert(startPos);
			break;
		}

		output.push_back(new ElseCond(cond, statement));
	}
	return output;
}

Statement* Parser::getElse(SymbTable* table) {
	if (currentToken._type != TokenType::ELSE)
		return nullptr;

	size_t startPos = currentPos;
	advance();

	Statement* statement = getStatement(table);
	if (statement == nullptr) {
		revert(startPos);
		return nullptr;
	}
	return statement;
}


template<SuppType T>
inline Expression<T>* Parser::getExpr(SymbTable* table)
{
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return nullptr;
	

	// order decides op-binding-strength strong -> weak 
	auto lvl1 = { TokenType::MUL, TokenType::DIV, TokenType::EQEQ };
	auto lvl2 = { TokenType::PLUS , TokenType::MINUS };
	tokenStream = addBrackets(tokenStream, lvl1);
	tokenStream = addBrackets(tokenStream, lvl2);

	// inserts Brackets around expr. for streamToExpr
	//tokenStream.insert(tokenStream.begin(), Token(LBRACKET));
	//tokenStream.insert(tokenStream.end(), Token(RBRACKET));
	return streamToExpr<T>(tokenStream, table);
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream() {
	size_t startPos = currentPos;
	size_t bracketSur = 0;
	bool mustValue = true;
	TokenType returnType = currentToken._type;
	while (returnType != TokenType::SEMICOLON &&
		!(returnType == TokenType::RBRACKET && bracketSur == 0)
		&& currentToken._type != TokenType::NONE) 
	{

		if (mustValue) {
			if (returnType != TokenType::LBRACKET) {
				if (returnType != TokenType::INTLIT && returnType != TokenType::DECLIT && returnType != TokenType::IDENTIFIER) {
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
				if (returnType != TokenType::PLUS && returnType != TokenType::MINUS && returnType != TokenType::MUL && returnType != TokenType::DIV && returnType != TokenType::EQEQ) {
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
		returnType = currentToken._type;
	}
	if (bracketSur > 0)
		return std::vector<Token>();

	return std::vector<Token>(tokenStream.begin() + startPos, tokenStream.begin() + currentPos);
}

std::vector<Token> Parser::addBrackets(std::vector<Token> tokenStream, std::initializer_list<TokenType> op)
{
	auto valExpr = { TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER };
	for (int i = 1; i < tokenStream.size(); i++) {
		// if token is a strong-binding operator
		if (std::find(op.begin(), op.end(), tokenStream[i]._type) != op.end()) {

			size_t bracketSur = 0;
			// places a LBracket
			for (int j = i - 1; j > -1; j--) {
				if (j == 0) {
					tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
					// i know its useless, but it looks good
					break;
				}
				else if (tokenStream[j]._type == TokenType::LBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
						bracketSur++;
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j]._type == TokenType::RBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j]._type) != valExpr.end()
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
				else if (tokenStream[j]._type == TokenType::RBRACKET) {
					if (bracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
						break;
					}
					bracketSur--;
				}
				else if (tokenStream[j]._type == TokenType::LBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j]._type) != valExpr.end()
					&& bracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
					break;
				}
			}
		}
	}
	return tokenStream;
}

template<SuppType T>
inline Expression<T>* Parser::streamToExpr(std::vector<Token> tokenStream, SymbTable* table)
{
	IEASTNode rootNode = IEASTNode(table);
	rootNode.name = "ad";
	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(&rootNode);
	for (Token token : tokenStream) {
		switch (token._type)
		{
		case TokenType::LBRACKET:
		{
			IEASTNode lNode = IEASTNode(table);
			nodeStack.top()->leftNode = &lNode;
			nodeStack.push(&lNode);
			break;
		}
		case TokenType::DECLIT:
		{
			IEASTNode* DecNode = nodeStack.top();
			nodeStack.pop();
			DecNode->token = new Token(token);
			break;
		}
		case TokenType::INTLIT:
		{
			IEASTNode* IntNode = nodeStack.top();
			nodeStack.pop();
			IntNode->token = new Token(token);
			break;
		}
		case TokenType::IDENTIFIER:
		{
			IEASTNode* varNode = nodeStack.top();
			nodeStack.pop();
			varNode->token = new Token(token);
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
			topNode->token = new Token(token);
			IEASTNode rightNode = IEASTNode(table);
			topNode->rightNode = &rightNode;
			nodeStack.push(&rightNode);
			break;
		}
	}
	if (nodeStack.size() > 0)
		throw std::invalid_argument("IEASTNode-Stack has elements over");
	rootNode.setReturnType(table);
	return rootNode.getExpr<T>();


	//if (tokenStream[0]._type == TokenType::LBRACKET) {
	//	// find end of bracket
	//	size_t eOB = 0;
	//	size_t surBrack = 0;
	//	for (size_t i = 1; i < tokenStream.size(); i++) {
	//		if (tokenStream[i]._type == TokenType::RBRACKET) {
	//			if (surBrack == 0) {
	//				eOB = i;
	//				break;
	//			}
	//			surBrack--;
	//		}
	//		else if (tokenStream[i]._type == TokenType::LBRACKET)
	//			surBrack++;
	//	}

	//	// when expression is surrounded by brackets
	//	if (eOB == tokenStream.size()-1) {
	//		auto cutStream = std::vector<Token>(tokenStream.begin() + 1, tokenStream.end() - 1);
	//		return streamToExpr(cutStream);
	//	}

	//	// when there is bin_op

	//}
}

