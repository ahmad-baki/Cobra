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

Statement* Parser::parse(std::vector<Token> tokens, Error& outError)
{
	BlockNode* block = new BlockNode();
	parse(tokens, block, outError);
	return block;
}

void Parser::parse(std::vector<Token> tokens, BlockNode* block, Error& outError)
{
	tokenStream = tokens;
	currentToken = tokenStream[0];
	revert(0);

	std::vector<Statement*> blockNode{};
	// main-parser loop
	while (currentPos < tokens.size()) {
		Statement* statement = getStatement(block->table, outError);
		if (statement == nullptr) {
			return;
		}
		blockNode.push_back(statement);
	}
	for (Statement* statement : blockNode) {
		block->add(statement);
	}
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

Statement* Parser::getStatement(SymbTable* table, Error& outError)
{
	if (currentToken.dataType == TokenType::NONE) {
		
		SyntaxError targetError = SyntaxError("Reached End of File while Parcing", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	Statement*(Parser:: *getStateFuncs[])(SymbTable* table, Error& outError) = {
		&Parser::getBlockState, 
		&Parser::getIfState,
		&Parser::getWhileState,
		&Parser::getDeclState,
		&Parser::getSetState,
		&Parser::getEmptyState,
		&Parser::getPrint
	};

	for (auto getStateFunc : getStateFuncs) {
		Error getStateError{};
		Statement* statement = (this->*getStateFunc)(table, getStateError);
		// when tere is a statement, that matches syntax
		if (statement != nullptr)
			return statement;

		// if there is a syntax-error
		if (outError.errorName != "NULL") {
			outError.copy(getStateError);
			return nullptr;
		}
	}
	Error targetError = SyntaxError("input doesnt fit any statement type", currentToken.line,
		currentToken.startColumn, currentToken.endColumn, path, text);
	outError.copy(targetError);
	return nullptr;
}

// Code-Block
Statement* Parser::getBlockState(SymbTable* table, Error& outError)
{
	if (currentToken.dataType != TokenType::LCURLBRACKET)
		return nullptr;

	size_t startPos = currentPos;
	advance();
	auto blockNode = new BlockNode(std::vector<Statement*>{}, table);
	while (currentToken.dataType != TokenType::RCURLBRACKET) {
		Statement* statement = getStatement(blockNode->table, outError);

		if (outError.errorName != "NULL") {
			return nullptr;
		}

		blockNode->add(statement);
	}

	advance();
	return blockNode;
}

// If-Statement
Statement* Parser::getIfState(SymbTable* table, Error& outError)
{
	if (currentToken.dataType != TokenType::IF)
		return nullptr;
	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		revert(startPos);
		return nullptr;
	}
	advance();

	Expression* cond = getExpr(table, outError);
	if (cond == nullptr) {
		return nullptr;
	}

	if (currentToken.dataType != TokenType::RBRACKET)
	{
		delete cond;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	Statement* statement = getStatement(table, outError);
	if (statement == nullptr) {
		delete cond;
		return nullptr;
	}

	std::vector<ElseCond*> ifElseStates = getIfElse(table, outError);

	Statement* elseState = getElse(table, outError);

	return new IfCond(cond, statement, ifElseStates, elseState);
}

// While-Loop
Statement* Parser::getWhileState(SymbTable* table, Error& outError)
{
	if (currentToken.dataType != TokenType::WHILE)
		return nullptr;

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();

	Expression* cond = getExpr(table, outError);
	if (cond == nullptr) {
		return nullptr;
	}

	if (currentToken.dataType != TokenType::RBRACKET) {
		delete cond;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	Statement* statement = getStatement(table, outError);

	if (statement == nullptr) {
		delete cond;
		return nullptr;
	}

	return new Loop(cond, statement);
}

// Decl (initialization)
Statement* Parser::getDeclState(SymbTable* table, Error& outError)
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
			SyntaxError targetError = SyntaxError("No type-specifier found", currentToken.line,
				currentToken.startColumn, currentToken.endColumn, path, text);
			outError.copy(targetError);
			return nullptr;
		}
		revert(startPos);
		return nullptr;
	}
	auto returnType = currentToken.dataType;
	advance();

	if (currentToken.dataType != TokenType::IDENTIFIER) {
		SyntaxError targetError = SyntaxError("No variablename found", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
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
		SyntaxError targetError = SyntaxError("Invalid Datatype", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
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
		expr = getExpr(table, outError);

		if (expr == nullptr) {
			return nullptr;
		}
	}
	else {
		delete expr;
		SyntaxError targetError = SyntaxError("Invalid Token", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	Statement* decl = new DeclVar(varName, table, currentToken.line, 
		tokenStream[startPos].startColumn, currentToken.endColumn, constVar, true, dataType, expr);
	return decl;
}

// setVar
Statement* Parser::getSetState(SymbTable* table, Error& outError)
{
	if (currentToken.dataType != TokenType::IDENTIFIER) {
		return nullptr;
	}
	std::string varName = currentToken.value;

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::EQ) {
		revert(startPos);
		return nullptr;
	}
	advance();
	// get expr
	Expression* expr = getExpr(table, outError);

	if (expr == nullptr)
		return nullptr;


	if (currentToken.dataType != TokenType::SEMICOLON) 
	{
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ';'", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	SetVar* setVar = new SetVar(varName, expr, table, currentToken.line,
		tokenStream[startPos].startColumn, currentToken.endColumn);

	advance();
	return setVar;
}

// Empty statement
Statement* Parser::getEmptyState(SymbTable* table, Error& outError)
{
	if (currentToken.dataType != TokenType::SEMICOLON)
		return nullptr;
	advance();
	return new BlockNode();
}

// Print statement
Statement* Parser::getPrint(SymbTable* table, Error& outError) 
{
	if (currentToken.dataType != TokenType::IDENTIFIER || 
		currentToken.value != "print")
		return nullptr;

	size_t startPos = currentPos;
	advance();

	if (currentToken.dataType != TokenType::LBRACKET) {
		SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	Expression* expr = getExpr(table, outError);

	if (expr == nullptr)
		return nullptr;

	if (currentToken.dataType != TokenType::RBRACKET) {
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	if (currentToken.dataType != TokenType::SEMICOLON) {
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ';'", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	return new PrintState(expr);
}

// list of else if statements
std::vector<ElseCond*> Parser::getIfElse(SymbTable* table, Error& outError)
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
			SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", currentToken.line,
				currentToken.startColumn, currentToken.endColumn, path, text);
			outError.copy(targetError);
			return {};
		}
		advance();

		Expression* cond = getExpr(table, outError);
		if (cond == nullptr){
			return {};
		}
		if (currentToken.dataType != TokenType::RBRACKET) {
			delete cond;
			SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", currentToken.line,
				currentToken.startColumn, currentToken.endColumn, path, text);
			outError.copy(targetError);
			return {};
		}

		advance();

		Statement* statement = getStatement(table, outError);
		if (statement == nullptr) {
			delete cond;
			return {};
		}
		output.push_back(new ElseCond(cond, statement, currentToken.line, 
			tokenStream[startPos].startColumn, currentToken.endColumn));
	}
	return output;
}

// else statement
Statement* Parser::getElse(SymbTable* table, Error& outError) {
	if (currentToken.dataType != TokenType::ELSE)
		return nullptr;

	size_t startPos = currentPos;
	advance();

	Statement* statement = getStatement(table, outError);
	if (statement == nullptr) {
		return nullptr;
	}
	return statement;
}

Expression* Parser::getExpr(SymbTable* table, Error& outError)
{
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0) {
		SyntaxError targetError = SyntaxError("Invalid Expression", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	tokenStream = transExprTokStream(tokenStream);
	IEASTNode rootNode = IEASTNode(table, path, text);
	streamToIEAST(tokenStream, table, rootNode, outError);

	if (outError.errorName != "NULL") {
		SyntaxError targetError = SyntaxError("Invalid Expression", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	Expression* expr = rootNode.getExpr(outError);
	return expr;
}

// delete the node, becouse its constructed on the heap
void Parser::getIEAST(SymbTable* table, IEASTNode& rootNode, Error& outError) {
	std::vector<Token> tokenStream = getExprTokStream();
	if (tokenStream.size() == 0){
		SyntaxError targetError = SyntaxError("Invalid Expression", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
		return;
	}

	tokenStream = transExprTokStream(tokenStream);
	streamToIEAST(tokenStream, table, rootNode, outError);
}

// delete the node, becouse its constructed on the heap
void Parser::streamToIEAST(std::vector<Token> tokenStream, SymbTable* table, IEASTNode& rootNode, Error& outError)
{
	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(&rootNode);
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
	if (nodeStack.size() > 0) {
		SyntaxError targetError = SyntaxError("IEASTNode - Stack has elements over", currentToken.line,
			currentToken.startColumn, currentToken.endColumn, path, text);
		outError.copy(targetError);
	}
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream() {
	size_t startPos = currentPos;
	size_t bracketSur = 0;
	bool mustValue = true;
	TokenType tokType = currentToken.dataType;

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

	while (tokType != TokenType::SEMICOLON && tokType != TokenType::COMMA &&
		!(tokType == TokenType::RBRACKET && bracketSur == 0)
		&& currentToken.dataType != TokenType::NONE) 
	{

		if (mustValue) {
			if (tokType != TokenType::LBRACKET) {
				if (std::find(valTypes.begin(), valTypes.end(), tokType) == valTypes.end()) {
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
			if (tokType != TokenType::RBRACKET) {
				if (std::find(binOps.begin(), binOps.end(), tokType) == binOps.end()) {
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
		tokType = currentToken.dataType;
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
