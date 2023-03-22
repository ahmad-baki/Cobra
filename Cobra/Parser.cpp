#include <stack>
#include <algorithm>
#include <stdexcept>
#include "Parser.h"
#include "printState.h"
#include "SyntaxError.h"
#include "Interpreter.h"



Parser::Parser(std::string_view text, std::string_view path)
	: text{ text }, path{ path }, currentPos { 0 }, currentToken{ Token(TokenType::NONE) }, 
	tokenStream{ std::vector<Token>() }
{
}

std::vector<Statement*> Parser::parse(std::vector<Token> tokens, Error& outError)
{
	tokenStream = tokens;
	currentToken = tokenStream[0];
	revert(0);

	std::vector<Statement*> statements{};
	// main-parser loop
	while (currentPos < tokens.size()) {
		Statement* statement = getStatement(outError);
		if (statement == nullptr) {
			return {};
		}
		statements.push_back(statement);
	}
	return statements;
}

void Parser::advance()
{
	currentPos++;
}

Token Parser::getCurrToken() {

	if (currentPos > tokenStream.size() - 1) {
		return Token(TokenType::NONE, currentToken.line, currentToken.endColumn + 1, 
			currentToken.endColumn + 2);
	}
	else {
		return tokenStream[currentPos];
	}
}

void Parser::revert(size_t pos) {
	currentPos = pos;
}

Statement* Parser::getStatement(Error& outError)
{
	if (getCurrToken().dataType == TokenType::NONE) {
		
		SyntaxError targetError = SyntaxError("Reached End of File while Parcing", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	Statement*(Parser:: *getStateFuncs[])(Error& outError) = {
		&Parser::getBlockState, 
		&Parser::getIfState,
		&Parser::getWhileState,
		&Parser::getDeclState,
		&Parser::getSetState,
		&Parser::getEmptyState,
		&Parser::getPrint
	};

	for (auto getStateFunc : getStateFuncs) {
		Statement* statement = (this->*getStateFunc)(outError);
		// when tere is a statement, that matches syntax
		if (statement != nullptr)
			return statement;

		// if there is a syntax-error
		if (outError.errorName != "NULL") {
			return nullptr;
		}
	}
	Error targetError = SyntaxError("input doesnt fit any statement type", getCurrToken().line,
		getCurrToken().startColumn, getCurrToken().endColumn, path, text);
	outError.copy(targetError);
	return nullptr;
}

// Code-Block
Statement* Parser::getBlockState(Error& outError)
{
	if (getCurrToken().dataType != TokenType::LCURLBRACKET)
		return nullptr;

	size_t startPos = currentPos;
	advance();
	auto blockNode = new BlockNode(std::vector<Statement*>{});
	while (getCurrToken().dataType != TokenType::RCURLBRACKET) {
		Statement* statement = getStatement(outError);

		if (outError.errorName != "NULL") {
			return nullptr;
		}

		blockNode->add(statement);
	}

	advance();
	return blockNode;
}

// If-Statement
Statement* Parser::getIfState(Error& outError)
{
	if (getCurrToken().dataType != TokenType::IF)
		return nullptr;
	size_t startPos = currentPos;
	advance();

	if (getCurrToken().dataType != TokenType::LBRACKET) {
		revert(startPos);
		return nullptr;
	}
	advance();

	Expression* cond = getExpr(outError);
	if (cond == nullptr) {
		return nullptr;
	}

	if (getCurrToken().dataType != TokenType::RBRACKET)
	{
		delete cond;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	Statement* statement = getStatement(outError);
	if (statement == nullptr) {
		delete cond;
		return nullptr;
	}

	std::vector<ElseCond*> ifElseStates = getIfElse(outError);

	Statement* elseState = getElse(outError);

	return new IfCond(cond, statement, ifElseStates, elseState);
}

// While-Loop
Statement* Parser::getWhileState(Error& outError)
{
	if (getCurrToken().dataType != TokenType::WHILE)
		return nullptr;

	size_t startPos = currentPos;
	advance();

	if (getCurrToken().dataType != TokenType::LBRACKET) {
		SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();

	Expression* cond = getExpr(outError);
	if (cond == nullptr) {
		return nullptr;
	}

	if (getCurrToken().dataType != TokenType::RBRACKET) {
		delete cond;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	Statement* statement = getStatement(outError);

	if (statement == nullptr) {
		delete cond;
		return nullptr;
	}

	return new Loop(cond, statement);
}

// Decl (initialization)
Statement* Parser::getDeclState(Error& outError)
{
	bool constVar{false};
	size_t startPos = currentPos;

	if (getCurrToken().dataType == TokenType::CONSTWORD) {
		constVar = true;
		advance();
	}
	if (getCurrToken().dataType != TokenType::INTWORD && 
		getCurrToken().dataType != TokenType::FLOATWORD &&
		getCurrToken().dataType != TokenType::CHARWORD &&
		getCurrToken().dataType != TokenType::VARWORD)
	{
		if (constVar) {
			SyntaxError targetError = SyntaxError("No type-specifier found", getCurrToken().line,
				getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			outError.copy(targetError);
			return nullptr;
		}
		revert(startPos);
		return nullptr;
	}
	int typeId{0};
	switch (getCurrToken().dataType)
	{
	case TokenType::INTWORD:
		typeId = Interpreter::getSingelton()->getTypeId("int", outError);
		break;
	case TokenType::FLOATWORD:
		typeId = Interpreter::getSingelton()->getTypeId("float", outError);
		break;
	case TokenType::CHARWORD:
		typeId = Interpreter::getSingelton()->getTypeId("char", outError);
		break;
	case TokenType::VARWORD:
		typeId = 0;
		break;
	default:
		SyntaxError targetError = SyntaxError("Invalid Datatype", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	bool isList = false;
	if (getCurrToken().dataType == TokenType::LSQBRACKET) {
		advance();
		if (getCurrToken().dataType != TokenType::RSQBRACKET) {
			SyntaxError targetError = SyntaxError("Presumably Missing ']'", getCurrToken().line,
				getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			outError.copy(targetError);
			return nullptr;
		}
		isList = true;
		advance();
	}

	if (getCurrToken().dataType != TokenType::IDENTIFIER) {
		SyntaxError targetError = SyntaxError("No variablename found", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	std::string varName = getCurrToken().value;
	advance();

	Expression* expr{ nullptr };
	// only decl
	if (getCurrToken().dataType == TokenType::SEMICOLON) {
		advance();
	}
	// decl + init
	else if (getCurrToken().dataType == TokenType::EQ) {
		advance();
		expr = getExpr(outError);

		if (expr == nullptr) {
			return nullptr;
		}
	}
	else {
		delete expr;
		SyntaxError targetError = SyntaxError("Invalid Token", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	
	if (getCurrToken().dataType != TokenType::SEMICOLON)
	{
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ';'", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	Statement* decl = new DeclVar(varName, getCurrToken().line,
		tokenStream[startPos].startColumn, getCurrToken().endColumn, constVar, true, typeId, isList, expr);
	return decl;
}

// setVar
Statement* Parser::getSetState(Error& outError)
{
	if (getCurrToken().dataType != TokenType::IDENTIFIER) {
		return nullptr;
	}
	std::string varName = getCurrToken().value;

	size_t startPos = currentPos;
	advance();

	// index should be checked here
	Expression* index;
	if (getCurrToken().dataType == TokenType::LSQBRACKET) {
		advance();
		index = getExpr(outError);
		if (index == nullptr) {
			return nullptr;
		}

		if (getCurrToken().dataType != TokenType::RSQBRACKET) {
			delete index;
			SyntaxError targetError = SyntaxError("Presumably Missing ']'", getCurrToken().line,
				getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			outError.copy(targetError);
			return nullptr;
		}
		advance();
	}
	else {
		index = nullptr;
	}

	if (getCurrToken().dataType != TokenType::EQ) {
		revert(startPos);
		return nullptr;
	}
	advance();
	// run expr
	Expression* expr = getExpr(outError);

	if (expr == nullptr)
		return nullptr;


	if (getCurrToken().dataType != TokenType::SEMICOLON) 
	{
		if (index != nullptr) {
			delete index;
		}
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ';'", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	SetVar* setVar = new SetVar(varName, index, expr, getCurrToken().line,
		tokenStream[startPos].startColumn, getCurrToken().endColumn);

	advance();
	return setVar;
}

// Empty statement
Statement* Parser::getEmptyState(Error& outError)
{
	if (getCurrToken().dataType != TokenType::SEMICOLON)
		return nullptr;
	advance();
	return new BlockNode();
}

// Print statement
Statement* Parser::getPrint(Error& outError) 
{
	if (getCurrToken().dataType != TokenType::IDENTIFIER || 
		getCurrToken().value != "print")
		return nullptr;

	size_t startPos = currentPos;
	advance();

	if (getCurrToken().dataType != TokenType::LBRACKET) {
		SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	Expression* expr = getExpr(outError);

	if (expr == nullptr)
		return nullptr;

	if (getCurrToken().dataType != TokenType::RBRACKET) {
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	if (getCurrToken().dataType != TokenType::SEMICOLON) {
		delete expr;
		SyntaxError targetError = SyntaxError("Presumably Missing ';'", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	return new PrintState(expr);
}

// list of else if statements
std::vector<ElseCond*> Parser::getIfElse(Error& outError)
{
	auto output = std::vector<ElseCond*>();
	while (getCurrToken().dataType == TokenType::ELSE) {
		size_t startPos = currentPos;
		advance();

		if (getCurrToken().dataType != TokenType::IF)
		{
			revert(startPos);
			break;
		}

		advance();

		if (getCurrToken().dataType != TokenType::LBRACKET) {
			SyntaxError targetError = SyntaxError("Presumably Missing '('-Bracket", getCurrToken().line,
				getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			outError.copy(targetError);
			return {};
		}
		advance();

		Expression* cond = getExpr(outError);
		if (cond == nullptr){
			return {};
		}
		if (getCurrToken().dataType != TokenType::RBRACKET) {
			delete cond;
			SyntaxError targetError = SyntaxError("Presumably Missing ')'-Bracket", getCurrToken().line,
				getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			outError.copy(targetError);
			return {};
		}

		advance();

		Statement* statement = getStatement(outError);
		if (statement == nullptr) {
			delete cond;
			return {};
		}
		output.push_back(new ElseCond(cond, statement, getCurrToken().line, 
			tokenStream[startPos].startColumn, getCurrToken().endColumn));
	}
	return output;
}

// else statement
Statement* Parser::getElse(Error& outError) {
	if (getCurrToken().dataType != TokenType::ELSE)
		return nullptr;

	size_t startPos = currentPos;
	advance();

	Statement* statement = getStatement(outError);
	if (statement == nullptr) {
		return nullptr;
	}
	return statement;
}

Expression* Parser::getExpr(Error& outError)
{
	std::vector<Token> tokenStream = getExprTokStream(currentPos, this->tokenStream);
	if (tokenStream.size() == 0) {
		SyntaxError targetError = SyntaxError("Invalid Expression", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	IEASTNode rootNode = IEASTNode(path, text);
	streamToIEAST(tokenStream, rootNode, outError);

	if (outError.errorName != "NULL") {
		SyntaxError targetError = SyntaxError("Invalid Expression", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return nullptr;
	}

	Expression* expr = rootNode.getExpr(outError);
	return expr;
}

// delete the node, becouse its constructed on the heap

void Parser::getIEAST(IEASTNode& rootNode, Error& outError) {
	std::vector<Token> tokenStream = getExprTokStream(currentPos, tokenStream);
	if (tokenStream.size() == 0){
		SyntaxError targetError = SyntaxError("Invalid Expression", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
		return;
	}

	tokenStream = transExprTokStream(tokenStream);
	streamToIEAST(tokenStream, rootNode, outError);
}


void Parser::streamToIEAST(std::vector<Token> tokenStream, IEASTNode& rootNode, Error& outError)
{
	// i dont fcking know why this is necc., but it works
	std::vector<Token>& refTokenStream = tokenStream;

	auto nodeStack = std::stack<IEASTNode*>();
	nodeStack.push(&rootNode);
	for (size_t i = 0; i < tokenStream.size(); i++) {
		switch (tokenStream[i].dataType)
		{
		case TokenType::LBRACKET:
		{
			IEASTNode* lNode = new IEASTNode(path, text);
			nodeStack.top()->leftNode = lNode;
			nodeStack.push(lNode);
			break;
		}
		case TokenType::DECLIT:
		case TokenType::INTLIT:
		case TokenType::CHARLIT:
		{
			IEASTNode* litNode = nodeStack.top();
			nodeStack.pop();
			litNode->token = tokenStream[i];
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
		case TokenType::LSQBRACKET:
		{
			i++;
			std::vector<Token> indexTokenStream = getExprTokStream(i, refTokenStream);

			if (refTokenStream[i].dataType != TokenType::RSQBRACKET) {
				SyntaxError targetError = SyntaxError("Presumably Missing ']'-Bracket", refTokenStream[i].line,
					refTokenStream[i].startColumn, refTokenStream[i].endColumn, path, text);
				outError.copy(targetError);
				return;
			}

			IEASTNode* indexRootNode = new IEASTNode(path, text);
			streamToIEAST(indexTokenStream, *indexRootNode, outError);
			auto top = nodeStack.top();
			nodeStack.top()->leftNode->leftNode = indexRootNode;
			break;
		}
		case TokenType::RSQBRACKET:
		{
			SyntaxError targetError = SyntaxError("Presumably Missing '['-Bracket", refTokenStream[i].line,
				refTokenStream[i].startColumn, refTokenStream[i].endColumn, path, text);
			outError.copy(targetError);
			return;
		}
		case TokenType::LCURLBRACKET:
		{
			IEASTNode* currNode = nodeStack.top();
			do
			{
				currNode->token = refTokenStream[i];
				i++;
				std::vector<Token> exprTokenStream = getExprTokStream(i, refTokenStream);
				if (exprTokenStream.size() == 0) {
					// todo
				}
				IEASTNode* valExpr = new IEASTNode(path, text);
				streamToIEAST(exprTokenStream, *valExpr, outError);
				currNode->leftNode = valExpr;
				IEASTNode* newNode = new IEASTNode(path, text);
				currNode->rightNode = newNode;
				currNode = newNode;

			} while (refTokenStream[i].dataType == TokenType::COMMA);

			if (refTokenStream[i].dataType != TokenType::RCURLBRACKET) {
				SyntaxError targetError = SyntaxError("Presumably Missing '}'-Bracket", refTokenStream[i].line,
					refTokenStream[i].startColumn, refTokenStream[i].endColumn, path, text);
				outError.copy(targetError);
				return;
			}
			currNode->token = refTokenStream[i];
			nodeStack.pop();
			break;
		}
		// operator
		default:
			IEASTNode* topNode = nodeStack.top();
			topNode->token = tokenStream[i];
			IEASTNode* rightNode = new IEASTNode(path, text);
			topNode->rightNode = rightNode;
			nodeStack.push(rightNode);
			break;
		}
	}
	if (nodeStack.size() > 0) {
		SyntaxError targetError = SyntaxError("IEASTNode - Stack has elements over", getCurrToken().line,
			getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		outError.copy(targetError);
	}
}

// finds token-string of expression and checks wether it 
// fits grammer of language
std::vector<Token> Parser::getExprTokStream(size_t& pos, const std::vector<Token>& tokenStream) {
	if (tokenStream[pos].dataType == TokenType::LCURLBRACKET) {
		return getArrExprTokStream(pos, tokenStream);
	}
	return getSingExprTokStream(pos, tokenStream);
}

std::vector<Token> Parser::getArrExprTokStream(size_t& pos, const std::vector<Token>& tokenStream) {
	size_t startPos = pos;
	std::vector<Token> out{};
	do
	{
		out.push_back(tokenStream[pos]);
		pos++;
		std::vector<Token> singTokenStream = getSingExprTokStream(pos, tokenStream);
		singTokenStream = transExprTokStream(singTokenStream);
		out.insert(out.end(), singTokenStream.begin(), singTokenStream.end());
	} while (tokenStream[pos].dataType == TokenType::COMMA);

	if (tokenStream[pos].dataType != TokenType::RCURLBRACKET) {
		pos = startPos;
		return {};
	}
	out.push_back(tokenStream[pos]);
	pos++;
	return out;
}

std::vector<Token> Parser::getSingExprTokStream(size_t& pos, const std::vector<Token>& tokenStream) {
	size_t startPos = pos;
	size_t bracketSur = 0;
	size_t sqrBracketSur = 0;
	bool mustValue = true;
	TokenType tokType;
	if (pos < tokenStream.size()) {
		tokType = tokenStream[pos].dataType;
	}
	else {
		tokType = TokenType::NONE;
	}

	std::vector<enum TokenType> valTypes{
		TokenType::INTLIT, TokenType::DECLIT, TokenType::CHARLIT , TokenType::IDENTIFIER
	};
	std::vector<enum TokenType> binOps{
		TokenType::PLUS, TokenType::MINUS, TokenType::MUL,
		TokenType::DIV, TokenType::EQEQ, TokenType::EXCLAEQ,
		TokenType::BIG, TokenType::BIGEQ, TokenType::SMALL,
		TokenType::SMALLEQ, TokenType::AND, TokenType::OR,
		TokenType::MOD,
	};

	while (tokType != TokenType::SEMICOLON && tokType != TokenType::COMMA &&
		tokType != TokenType::RCURLBRACKET &&
		!(tokType == TokenType::RBRACKET && bracketSur == 0) &&
		!(tokType == TokenType::RSQBRACKET && sqrBracketSur == 0)
		&& tokenStream[pos].dataType != TokenType::NONE)
	{

		if (mustValue) {
			if (tokType != TokenType::LBRACKET) {
				if (std::find(valTypes.begin(), valTypes.end(), tokType) == valTypes.end()) {
					//revert(startPos);
					pos = startPos;
					tokType = tokenStream[pos].dataType;
					return std::vector<Token>();
				}
				mustValue = false;
			}
			else {
				bracketSur++;
			}
		}
		else {
			if (tokType == TokenType::RSQBRACKET) {
				sqrBracketSur--;
			}
			else if (tokType != TokenType::RBRACKET) {
				if (tokType == TokenType::LSQBRACKET) {
					sqrBracketSur++;
				}
				else if (std::find(binOps.begin(), binOps.end(), tokType) == binOps.end()) {
					//revert(startPos);
					pos = startPos;
					tokType = tokenStream[pos].dataType;
					return std::vector<Token>();
				}
				mustValue = true;
			}
			else {
				bracketSur--;
			}
		}
		// advance();
		pos++;
		if (pos < tokenStream.size()) {
			tokType = tokenStream[pos].dataType;
		}
		else {
			tokType = TokenType::NONE;
		}
	}
	if (bracketSur > 0 || sqrBracketSur > 0)
		return std::vector<Token>();

	auto outTokenStream = std::vector<Token>(tokenStream.begin() + startPos, tokenStream.begin() + pos);
	return transExprTokStream(outTokenStream);
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

	// just to be sure
	tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
	tokenStream.push_back(Token(TokenType::RBRACKET));

	for (auto opClass : opClasses) {
		tokenStream = addBrackets(tokenStream, opClass);
	}
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
			size_t sqrBracketSur = 0;
			// places a LBracket
			for (int j = i - 1; j > -1; j--) {
				if (j == 0) {
					tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
					// i know its useless, but it looks good
					break;
				}
				else if (tokenStream[j].dataType == TokenType::LBRACKET) {
					bracketSur--;
					if (bracketSur == 0 && sqrBracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
						bracketSur++;
						break;
					}
				}
				else if (tokenStream[j].dataType == TokenType::LSQBRACKET) {
					sqrBracketSur--;
				}
				else if (tokenStream[j].dataType == TokenType::RBRACKET) {
					bracketSur++;
				}
				else if (tokenStream[j].dataType == TokenType::RSQBRACKET) {
					sqrBracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].dataType) != valExpr.end()
					&& bracketSur == 0 && sqrBracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
					bracketSur++;
					break;
				}
			}
			// becouse there is now a lBracket left from i
			i++;
			sqrBracketSur = 0;
			bracketSur = 0;
			// places a RBracket
			for (int j = i + 1; j < tokenStream.size(); j++) {
				if (j == tokenStream.size() - 1) {
					tokenStream.insert(tokenStream.end(), Token(TokenType::RBRACKET));
					break;
				}
				else if (tokenStream[j].dataType == TokenType::RBRACKET) {
					//if (bracketSur == 0) {
					//	tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
					//	break;
					//}
					bracketSur--;
					if (bracketSur == 0 && sqrBracketSur == 0) {
						tokenStream.insert(tokenStream.begin() + j + 1, Token(TokenType::RBRACKET));
						break;
					}
				}
				else if (tokenStream[j].dataType == TokenType::RSQBRACKET) {
					//if (bracketSur == 0) {
					//	tokenStream.insert(tokenStream.begin() + j, Token(TokenType::RBRACKET));
					//	break;
					//}
					sqrBracketSur--;
					//if (bracketSur == 0 && sqrBracketSur == 0) {
					//	tokenStream.insert(tokenStream.begin() + j + 1, Token(TokenType::RBRACKET));
					//	break;
					//}
				}
				else if (tokenStream[j].dataType == TokenType::LBRACKET) {
					bracketSur++;
				}
				else if (tokenStream[j].dataType == TokenType::LSQBRACKET) {
					bracketSur++;
				}
				else if (std::find(valExpr.begin(), valExpr.end(), tokenStream[j].dataType) != valExpr.end()
					&& bracketSur == 0 && sqrBracketSur == 0) {
					tokenStream.insert(tokenStream.begin() + j+1, Token(TokenType::RBRACKET));
					break;
				}
			}
		}
	}
	return tokenStream;
}
