#include <stack>
#include <algorithm>
#include <stdexcept>
#include "Parser.h"
#include "printState.h"



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
	//block->table->clearReg();

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
	if (currentToken._type == TokenType::NONE)
		return nullptr;

	Statement* (Parser:: *statementTypes[])(SymbTable * table) = {
		&Parser::getBlockState, 
		&Parser::getIfState,
		&Parser::getWhileState,
		&Parser::getDeclState,
		&Parser::getSetState,
		&Parser::getEmptyState,
		&Parser::getPrint
	};

	for (auto statementType : statementTypes) {
		Statement* statement = (this->*statementType)(table);
		if (statement != nullptr)
			return statement;
	}

	return nullptr;
}

// Code-Block
Statement* Parser::getBlockState(SymbTable* table)
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
		blockNode->add(statement);
		//advance();
	}
	advance();
	blockNode->table->clearReg();
	return blockNode;
}

// If-Statement
Statement* Parser::getIfState(SymbTable* table)
{
	if (currentToken._type != TokenType::IF)
		return nullptr;
	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::LBRACKET) {
		revert(startPos);
		return nullptr;
	}
	advance();

	Expression<bool>* cond = getExpr<bool>(table);
	if (cond == nullptr) {
		revert(startPos);
		return nullptr;
	}
	if (currentToken._type != TokenType::RBRACKET) {
		revert(startPos);
		return nullptr;
	}

	advance();

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
Statement* Parser::getWhileState(SymbTable* table)
{
	if (currentToken._type != TokenType::WHILE)
		return nullptr;
	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::LBRACKET) {
		revert(startPos);
		return nullptr;
	}
	advance();

	Expression<bool>* cond = getExpr<bool>(table);
	if (cond == nullptr) {
		revert(startPos);
		return nullptr;
	}
	if (currentToken._type != TokenType::RBRACKET) {
		revert(startPos);
		return nullptr;
	}

	advance();

	Statement* statement = getStatement(table);
	if (statement == nullptr) {
		revert(startPos);
		return nullptr;
	}

	return new Loop(cond, statement);
}

// Decl (initialization)
Statement* Parser::getDeclState(SymbTable* table)
{
	if (currentToken._type != TokenType::INTWORD && currentToken._type != TokenType::FLOATWORD) {
		return nullptr;
	}
	auto returnType = currentToken._type;

	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::IDENTIFIER) {
		revert(startPos);
		return nullptr;
	}
	std::string varName = currentToken._value;


	if (returnType == TokenType::INTWORD) {
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
			Statement* setVar = getSetState(table);
			return new BlockNode(std::vector<Statement*>{
				decl, setVar
			}, nullptr);
		}
	}
	else if (returnType == TokenType::FLOATWORD) {
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
			Statement* setVar = getSetState(table);
			return new BlockNode(std::vector<Statement*>{
				decl, setVar
			}, nullptr);
		}
	}

	revert(startPos);
	return nullptr;
}

// setVar
Statement* Parser::getSetState(SymbTable* table)
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

		if (expr == nullptr)
			throw std::invalid_argument("no expr found");

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
Statement* Parser::getEmptyState(SymbTable* table)
{
	if (currentToken._type != TokenType::SEMICOLON)
		return nullptr;
	advance();
	return new BlockNode();
}

// Print statement
Statement* Parser::getPrint(SymbTable* table) 
{
	if (currentToken._type != TokenType::IDENTIFIER && 
		currentToken._value == "print")
		return nullptr;

	size_t startPos = currentPos;
	advance();

	if (currentToken._type != TokenType::LBRACKET) {
		revert(startPos);
		return nullptr;
	}
	advance();
	IEASTNode& rootNode = *getIEAST(table);

	// int
	if (rootNode.getReturnType() == 0) {
		if (currentToken._type != TokenType::RBRACKET) {
			revert(startPos);
			return nullptr;
		}
		advance();
		if (currentToken._type != TokenType::SEMICOLON) {
			revert(startPos);
			return nullptr;
		}
		advance();
		return new PrintState(rootNode.getExpr<int>());
	}

	// float
	if (rootNode.getReturnType() == 1) {
		if (currentToken._type != TokenType::RBRACKET) {
			revert(startPos);
			return nullptr;
		}
		advance();
		if (currentToken._type != TokenType::SEMICOLON) {
			revert(startPos);
			return nullptr;
		}
		advance();
		return new PrintState(rootNode.getExpr<float>());
	}

	// its constructed on the heap
	delete &rootNode;
}

std::vector<ElseCond*> Parser::getIfElse(SymbTable* table)
{
	auto output = std::vector<ElseCond*>();
	while (currentToken._type == TokenType::ELSE) {
		size_t startPos = currentPos;
		advance();

		if (currentToken._type != TokenType::IF)
		{
			revert(startPos);
			break;
		}

		advance();

		if (currentToken._type != TokenType::LBRACKET) {
			revert(startPos);
			break;
		}
		advance();

		Expression<bool>* cond = getExpr<bool>(table);
		if (cond == nullptr) {
			revert(startPos);
			break;
		}
		if (currentToken._type != TokenType::RBRACKET) {
			revert(startPos);
			break;
		}

		advance();

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

	tokenStream = transExprTokStream(tokenStream);
	IEASTNode& rootNode = *streamToIEAST(tokenStream, table);
	auto expr = rootNode.getExpr<T>();
	delete &rootNode;
	return expr;
}

// delete the node, becouse its constructed on the heap
IEASTNode* Parser::getIEAST(SymbTable* table) {
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0)
		return nullptr;

	tokenStream = transExprTokStream(tokenStream);

	return streamToIEAST(tokenStream, table);
}

// delete the node, becouse its constructed on the heap
IEASTNode* Parser::streamToIEAST(std::vector<Token> tokenStream, SymbTable* table)
{
	IEASTNode* rootNode = new IEASTNode(table);
	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(rootNode);
	for (size_t i = 0; i < tokenStream.size(); i++) {
		switch (tokenStream[i]._type)
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
		throw std::invalid_argument("IEASTNode-Stack has elements over");
	rootNode->setReturnType(table);
	return rootNode;
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream() {
	size_t startPos = currentPos;
	size_t bracketSur = 0;
	bool mustValue = true;
	TokenType returnType = currentToken._type;
	std::vector<enum TokenType> valTypes{
		TokenType::INTLIT, TokenType::DECLIT, TokenType::IDENTIFIER
	};
	std::vector<enum TokenType> binOps{
		TokenType::PLUS, TokenType::MINUS, TokenType::MUL,
		TokenType::DIV, TokenType::EQEQ, TokenType::EXCLAEQ
	};

	while (returnType != TokenType::SEMICOLON &&
		!(returnType == TokenType::RBRACKET && bracketSur == 0)
		&& currentToken._type != TokenType::NONE) 
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
		returnType = currentToken._type;
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
					tokenStream.insert(tokenStream.begin() + j+1, Token(TokenType::RBRACKET));
					break;
				}
			}
		}
	}
	return tokenStream;
}
