#include <stack>
#include <algorithm>
#include <stdexcept>
#include <string>
#include "Parser.h"
#include "printState.h"
#include "SyntaxError.h"
#include "Interpreter.h"
#include "ExprState.h"
#include "IEASTFuncNode.h"
#include "DeclFunc.h"
#include "BreakState.h"


Parser::Parser(/*std::string_view text, std::string_view path*/)
	: /*text{text}, path{path}, */ currentPos{0}, tokenStream{ std::vector<Token>() }
{
}

std::vector<Statement*> Parser::parse(std::vector<Token> tokens, Error& outError)
{
	if (tokens.size() == 0) {
		return {};
	}

	tokenStream = tokens;
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
	if (currentPos != tokenStream.size()) {
		currentPos++;
	}
}

Token Parser::getCurrToken() {

	if (currentPos > tokenStream.size() - 1) {
		std::string_view text{};
		fs::path path{};
		if (tokenStream.size() != 0) {
			const Token& currentToken = tokenStream[currentPos - 1];
			return Token(TokenType::NONE, currentToken.path, currentToken.text,
				currentToken.line, currentToken.endColumn, currentToken.endColumn + 1);
		}
		return Token(TokenType::NONE);
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Reached End of File while Parcing", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
		outError.copy(targetError);
		return nullptr;
	}

	Statement*(Parser:: *getStateFuncs[])(Error& outError) = {
		&Parser::getBlockState, 
		&Parser::getIfState,
		&Parser::getWhileState,
		&Parser::getFuncDeclState,
		&Parser::getDeclState,
		&Parser::getSetState,
		&Parser::getEmptyState,
		//&Parser::getPrint,
		&Parser::getExprState,
		&Parser::getReturnState
	};

	for (auto getStateFunc : getStateFuncs) {
		Statement* statement = (this->*getStateFunc)(outError);
		// when tere is a statement, that matches syntax
		if (statement != nullptr)
			return statement;

		// if there is a syntax-error
		if (outError.errType != ErrorType::NULLERROR) {
			return nullptr;
		}
	}
	Token tok = getCurrToken();
	Error targetError(ErrorType::SYNTAXERROR, "input doesnt fit any statement type", tok.line,
		tok.startColumn, tok.endColumn, tok.path, tok.text);
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

		if (outError.errType != ErrorType::NULLERROR) {
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ')'-Bracket", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '('-Bracket", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ')'-Bracket", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
	return getDeclStateCustSep(outError, { TokenType::SEMICOLON });
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
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ']'", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ';'", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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

// Expression Statement
Statement* Parser::getExprState(Error& outError) {
	size_t startPos = currentPos;
	Error tempError{};
	Expression* expr = getExpr(tempError);

	if (expr == nullptr) {
		revert(startPos);
		return nullptr;
	}

	if (getCurrToken().dataType != TokenType::SEMICOLON) {
		delete expr;
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ';'", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
		outError.copy(targetError);
		return nullptr;
	}
	advance();
	return new ExprState(expr);
}

// Decl-Func Statement
Statement* Parser::getFuncDeclState(Error& outError) {
	int typeId;
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
	case TokenType::VOIDWORD:
		typeId = -1;
		break;
	default:
		return nullptr;
	}

	size_t startPos = currentPos;
	advance();
	
	bool isList = false;
	if (getCurrToken().dataType == TokenType::LSQBRACKET) {
		advance();

		if (getCurrToken().dataType != TokenType::RSQBRACKET) {
			//Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ']'", getCurrToken().line,
			//	getCurrToken().startColumn, getCurrToken().endColumn, path, text);
			//outError.copy(targetError);
			return nullptr;
		}
		isList = true;
		advance();
	}

	if (getCurrToken().dataType != TokenType::IDENTIFIER) {
		//Error targetError(ErrorType::SYNTAXERROR, "Missing Identifier", getCurrToken().line,
		//	getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		//outError.copy(targetError);
		revert(startPos);
		return nullptr;
	}

	std::string funcName = getCurrToken().value;
	
	advance();

	if (getCurrToken().dataType != TokenType::LBRACKET) {
		//Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '('", getCurrToken().line,
		//	getCurrToken().startColumn, getCurrToken().endColumn, path, text);
		//outError.copy(targetError);
		revert(startPos);
		return nullptr;
	}
	advance();
	
	std::vector<DeclVar*> params{};
	while (true) {
		params.push_back(getDeclStateCustSep(outError, { }));

		if (outError.errType != ErrorType::NULLERROR) {
			return nullptr;
		}

		if (getCurrToken().dataType == TokenType::RBRACKET) {
			break;
		}

		if (getCurrToken().dataType != TokenType::COMMA) {
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ','", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
			outError.copy(targetError);
			return nullptr;
		}
		advance();
	}

	advance();
	Statement* statement = getStatement(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return nullptr;
	}

	return new DeclFunc(funcName, typeId, isList, params, statement);
}

DeclVar* Parser::getDeclStateCustSep(Error& outError, std::vector<enum TokenType> sep) {
	bool constVar{ false };
	size_t startPos = currentPos;

	if (getCurrToken().dataType == TokenType::CONSTWORD) {
		constVar = true;
		advance();
	}
	if (getCurrToken().dataType != TokenType::BOOLWORD &&
		getCurrToken().dataType != TokenType::INTWORD &&
		getCurrToken().dataType != TokenType::FLOATWORD &&
		getCurrToken().dataType != TokenType::CHARWORD &&
		getCurrToken().dataType != TokenType::STRINGWORD &&
		getCurrToken().dataType != TokenType::VARWORD)
	{
		if (constVar) {
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "No type-specifier found", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
			outError.copy(targetError);
			return nullptr;
		}
		revert(startPos);
		return nullptr;
	}
	int typeId{ 0 };
	switch (getCurrToken().dataType)
	{
	case TokenType::BOOLWORD:
		typeId = Interpreter::getSingelton()->getTypeId("bool", outError);
		break;
	case TokenType::INTWORD:
		typeId = Interpreter::getSingelton()->getTypeId("int", outError);
		break;
	case TokenType::FLOATWORD:
		typeId = Interpreter::getSingelton()->getTypeId("float", outError);
		break;
	case TokenType::CHARWORD:
		typeId = Interpreter::getSingelton()->getTypeId("char", outError);
		break;
	case TokenType::STRINGWORD:
		typeId = Interpreter::getSingelton()->getTypeId("string", outError);
		break;
	case TokenType::VARWORD:
		typeId = 0;
		break;
	default:
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Invalid Datatype", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
		outError.copy(targetError);
		return nullptr;
	}

	advance();

	bool isList = false;
	if (getCurrToken().dataType == TokenType::LSQBRACKET) {
		advance();
		if (getCurrToken().dataType != TokenType::RSQBRACKET) {
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ']'", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
			outError.copy(targetError);
			return nullptr;
		}
		isList = true;
		advance();
	}

	if (getCurrToken().dataType != TokenType::IDENTIFIER) {
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "No variablename found", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
		outError.copy(targetError);
		return nullptr;
	}
	std::string varName = getCurrToken().value;
	advance();

	Expression* expr{ nullptr };
	//// only decl
	//if (getCurrToken().dataType == sep) {
	//	advance();
	//}
	// decl + init
	if (getCurrToken().dataType == TokenType::EQ) {
		advance();
		expr = getExpr(outError);

		if (expr == nullptr) {
			return nullptr;
		}
	}
	//else {
	//	delete expr;
	//	Error targetError(ErrorType::SYNTAXERROR, "Invalid Token", getCurrToken().line,
	//		getCurrToken().startColumn, getCurrToken().endColumn, path, text);
	//	outError.copy(targetError);
	//	return nullptr;
	//}

	// if sep is needed
	if (sep.size() > 0) {
		// if end of decl-state is without sep
		if (std::find(sep.begin(), sep.end(), getCurrToken().dataType) == sep.end())
		{
			delete expr;
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Invalid Token", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
			outError.copy(targetError);
			return nullptr;
		}
		advance();
	}
	DeclVar* decl = new DeclVar(varName, getCurrToken().line,
		tokenStream[startPos].startColumn, getCurrToken().endColumn, constVar, true, typeId, isList, expr);
	return decl;
}

// Print statement
//Statement* Parser::getPrint(Error& outError) 
//{
//	if (getCurrToken().dataType != TokenType::IDENTIFIER || 
//		getCurrToken().value != "print")
//		return nullptr;
//
//	size_t startPos = currentPos;
//	advance();
//
//	if (getCurrToken().dataType != TokenType::LBRACKET) {
//		Token tok = getCurrToken();
//		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '('-Bracket", tok.line,
//			tok.startColumn, tok.endColumn, tok.path, tok.text);
//		outError.copy(targetError);
//		return nullptr;
//	}
//	advance();
//	Expression* expr = getExpr(outError);
//
//	if (expr == nullptr)
//		return nullptr;
//
//	if (getCurrToken().dataType != TokenType::RBRACKET) {
//		delete expr;
//		Token tok = getCurrToken();
//		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ')'-Bracket", tok.line,
//			tok.startColumn, tok.endColumn, tok.path, tok.text);
//		outError.copy(targetError);
//		return nullptr;
//	}
//	advance();
//	if (getCurrToken().dataType != TokenType::SEMICOLON) {
//		delete expr;
//		Token tok = getCurrToken();
//		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ';'", tok.line,
//			tok.startColumn, tok.endColumn, tok.path, tok.text);
//		outError.copy(targetError);
//		return nullptr;
//	}
//	advance();
//	return new PrintState(expr);
//}

Statement* Parser::getReturnState(Error& outError) {
	if (getCurrToken().dataType != TokenType::RETURN) {
		return nullptr;
	}
	size_t startPos = currentPos;
	advance();

	BreakState* breakState = new BreakState(4294967295);
	if (getCurrToken().dataType == TokenType::SEMICOLON) {
		return breakState;
	}

	Expression* expr = getExpr(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return nullptr;
	}

	SetVar* setVar = new SetVar("$return", nullptr, expr, 
		expr->line, startPos, expr->endColumn);
	return new BlockNode({ setVar, breakState }, "return");
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
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '('-Bracket", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
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
			Token tok = getCurrToken();
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ')'-Bracket", tok.line,
				tok.startColumn, tok.endColumn, tok.path, tok.text);
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
	int brackCount = getBrackCount(tokenStream);
	if (tokenStream.size() == 0) {
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Invalid Expression", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
		outError.copy(targetError);
		return nullptr;
	}

	IEASTNode rootNode = IEASTNode(getCurrToken().path, getCurrToken().text);
	streamToIEAST(tokenStream, rootNode, outError);

	if (outError.errType != ErrorType::NULLERROR) {
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Invalid Expression", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "Invalid Expression", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
			IEASTNode* lNode = new IEASTNode(getCurrToken().path, getCurrToken().text);
			nodeStack.top()->leftNode = lNode;
			nodeStack.push(lNode);
			break;
		}
		case TokenType::BOOLLIT:
		case TokenType::INTLIT:
		case TokenType::DECLIT:
		case TokenType::CHARLIT:
		case TokenType::STRINGLIT:
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
		case TokenType::FUNC:
		{
			IEASTNode* node = nodeStack.top();
			nodeStack.pop();
			node->token = tokenStream[i];
			i++;

			IEASTFuncNode* funcNode = new IEASTFuncNode(node->path, node->text);
			node->leftNode = funcNode;
			// identifier
			funcNode->token = tokenStream[i];
			i+=2;

			std::vector<IEASTNode*> params{};
			while (true) {
				std::vector<Token> paramTokens = getBraSingExprTokStream(i, tokenStream);

				if (paramTokens.size() == 0) {
					Error targetError(ErrorType::SYNTAXERROR, "Invalid Expression", funcNode->token.line,
						funcNode->token.startColumn, tokenStream[i].endColumn, 
						tokenStream[i].path, tokenStream[i].text);
					outError.copy(targetError);
					return;
				}
				IEASTNode* paramRootNode = new IEASTNode(getCurrToken().path, getCurrToken().text);
				streamToIEAST(paramTokens, *paramRootNode, outError);
				params.push_back(paramRootNode);

				if (tokenStream[i].dataType == TokenType::RBRACKET) {
					break;
				}

				if (tokenStream[i].dataType != TokenType::COMMA) {
					Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ','-Bracket", tokenStream[i].line,
						tokenStream[i].startColumn, tokenStream[i].endColumn, tokenStream[i].path, tokenStream[i].text);
					outError.copy(targetError);
					return;
				}
				i++;
			}
			funcNode->params = params;
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
				Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ']'-Bracket", refTokenStream[i].line,
					refTokenStream[i].startColumn, refTokenStream[i].endColumn, 
					refTokenStream[i].path, refTokenStream[i].text);
				outError.copy(targetError);
				return;
			}

			IEASTNode* indexRootNode = new IEASTNode(getCurrToken().path, getCurrToken().text);
			streamToIEAST(indexTokenStream, *indexRootNode, outError);
			auto top = nodeStack.top();
			nodeStack.top()->leftNode->leftNode = indexRootNode;
			break;
		}
		case TokenType::RSQBRACKET:
		{
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '['-Bracket", refTokenStream[i].line,
				refTokenStream[i].startColumn, refTokenStream[i].endColumn, refTokenStream[i].path, 
				refTokenStream[i].text);
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
				IEASTNode* valExpr = new IEASTNode(getCurrToken().path, getCurrToken().text);
				streamToIEAST(exprTokenStream, *valExpr, outError);
				currNode->leftNode = valExpr;
				IEASTNode* newNode = new IEASTNode(getCurrToken().path, getCurrToken().text);
				currNode->rightNode = newNode;
				currNode = newNode;

			} while (refTokenStream[i].dataType == TokenType::COMMA);

			if (refTokenStream[i].dataType != TokenType::RCURLBRACKET) {
				Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing '}'-Bracket", refTokenStream[i].line,
					refTokenStream[i].startColumn, refTokenStream[i].endColumn, refTokenStream[i].path, 
					refTokenStream[i].text);
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
			IEASTNode* rightNode = new IEASTNode(getCurrToken().path, getCurrToken().text);
			topNode->rightNode = rightNode;
			nodeStack.push(rightNode);
			break;
		}
	}
	if (nodeStack.size() > 0) {
		Token tok = getCurrToken();
		Error targetError(ErrorType::SYNTAXERROR, "IEASTNode - Stack has elements over", tok.line,
			tok.startColumn, tok.endColumn, tok.path, tok.text);
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
		TokenType::BOOLLIT, TokenType::INTLIT, TokenType::DECLIT, 
		TokenType::CHARLIT, TokenType::STRINGLIT , TokenType::IDENTIFIER
	};
	std::vector<enum TokenType> binOps{
		TokenType::PLUS, TokenType::MINUS, TokenType::MUL,
		TokenType::DIV, TokenType::EQEQ, TokenType::EXCLAEQ,
		TokenType::BIG, TokenType::BIGEQ, TokenType::SMALL,
		TokenType::SMALLEQ, TokenType::AND, TokenType::OR,
		TokenType::MOD,
	};

	auto outTokenStream = std::vector<Token>();

	while (tokType != TokenType::SEMICOLON && tokType != TokenType::COMMA &&
		tokType != TokenType::RCURLBRACKET &&
		!(tokType == TokenType::RBRACKET && bracketSur == 0) &&
		!(tokType == TokenType::RSQBRACKET && sqrBracketSur == 0)
		&& tokType != TokenType::NONE)
	{

		if (mustValue) {
			if (tokType != TokenType::LBRACKET) {
				Error outError{};
				std::vector<Token> funkTokStr = getFuncTokStream(tokenStream, pos, outError);
				if (funkTokStr.size() > 0) {
					int brackCount = getBrackCount(funkTokStr);
					outTokenStream.insert(outTokenStream.end(), funkTokStr.begin(), funkTokStr.end());
					//bracketSur++;
					mustValue = false;
				}
				else if (tokType == TokenType::MINUS) {
					outTokenStream.push_back(Token(TokenType::INTLIT, "", "", 0, 0, 0, "0"));
					outTokenStream.push_back(tokenStream[pos]);
				}
				else if (std::find(valTypes.begin(), valTypes.end(), tokType) == valTypes.end()) {
					//revert(startPos);
					pos = startPos;
					tokType = tokenStream[pos].dataType;
					return std::vector<Token>();
				}
				else {
					outTokenStream.push_back(tokenStream[pos]);
					mustValue = false;
				}
				//outTokenStream.push_back(tokenStream[pos]);
			}
			else {
				bracketSur++;
				outTokenStream.push_back(tokenStream[pos]);
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
		outTokenStream.push_back(tokenStream[pos]);
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

	//auto outTokenStream = std::vector<Token>(tokenStream.begin() + startPos, tokenStream.begin() + pos);
	return transExprTokStream(outTokenStream);
}

std::vector<Token> Parser::getFuncTokStream(const std::vector<Token>& tokenStream, size_t& pos, Error& outError)
{
	if (tokenStream[pos].dataType != TokenType::IDENTIFIER) {
		return {};
	}

	if (tokenStream.size() - pos < 3) {
		return {};
	}

	size_t startPos = pos;
	std::vector<Token> out{ Token(TokenType::FUNC), tokenStream[pos] };

	pos++;
	if (tokenStream[pos].dataType != TokenType::LBRACKET) {
		pos = startPos;
		return {};
	}

	out.push_back(tokenStream[pos]);
	pos++;

	while (pos < tokenStream.size()) {
		auto tokStream = getSingExprTokStream(pos, tokenStream);
		if (tokStream.size() == 0) {
			Error targetError(ErrorType::SYNTAXERROR, "Invalid Expression as parameter", tokenStream[startPos].line,
				tokenStream[startPos].startColumn, tokenStream[pos].endColumn);
			outError.copy(targetError);
			return {};
		}

		out.insert(out.end(), tokStream.begin(), tokStream.end());

		if (tokenStream[pos].dataType == TokenType::RBRACKET) {
			break;
		}

		if (tokenStream[pos].dataType != TokenType::COMMA) {
			Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ','", tokenStream[pos].line,
				tokenStream[pos].startColumn, tokenStream[pos].endColumn);
			outError.copy(targetError);
			return {};
		}
		out.push_back(tokenStream[pos]);
		pos++;
	}

	if (tokenStream[pos].dataType != TokenType::RBRACKET) {
		Error targetError(ErrorType::SYNTAXERROR, "Presumably Missing ')'", tokenStream[pos].line,
			tokenStream[pos].startColumn, tokenStream[pos].endColumn);
		outError.copy(targetError);
		return {};
	}
	out.push_back(tokenStream[pos]);
	out[0].value = std::to_string(pos);
	return out;
}

std::vector<Token> Parser::getBraSingExprTokStream(size_t& pos, const std::vector<Token>& tokenStream)
{
	size_t startPos = pos;
	size_t brackSur = 1;
	pos++;
	while(brackSur != 0){
		if (tokenStream[pos].dataType == TokenType::LBRACKET) {
			brackSur++;
		}
		else if (tokenStream[pos].dataType == TokenType::RBRACKET) {
			brackSur--;
		}
		pos++;
	}
	return {tokenStream.begin() + startPos, tokenStream.begin() + pos};
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
	auto valExpr = { TokenType::BOOLLIT, TokenType::INTLIT, TokenType::DECLIT, 
		TokenType::CHARLIT, TokenType::STRINGLIT, TokenType::IDENTIFIER };
	for (int i = 1; i < tokenStream.size(); i++) {
		// if token is a strong-binding operator
		if (std::find(op.begin(), op.end(), tokenStream[i].dataType) != op.end()) {

			size_t bracketSur = 0;
			size_t sqrBracketSur = 0;
			// places a LBracket
			for (int j = i - 1; j > -1; j--) {
				if (j == 0) {
					tokenStream.insert(tokenStream.begin(), Token(TokenType::LBRACKET));
				}
				else if (tokenStream[j].dataType == TokenType::LBRACKET) {
					bracketSur--;					
					
					// funcs
					if (j > 2 && tokenStream[j-1].dataType == TokenType::IDENTIFIER
						&& tokenStream[j - 2].dataType == TokenType::FUNC) {
						tokenStream.insert(tokenStream.begin() + j - 2, Token(TokenType::LBRACKET));
						break;
					}

					// normal
					else if (bracketSur == 0 && sqrBracketSur == 0) {
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

					// funcs
					if (tokenStream[j].dataType == TokenType::IDENTIFIER && j > 0
						&& tokenStream[j - 1].dataType == TokenType::FUNC) {
						tokenStream.insert(tokenStream.begin() + j - 1, Token(TokenType::LBRACKET));
					}
					else {
						tokenStream.insert(tokenStream.begin() + j, Token(TokenType::LBRACKET));
					}
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
				}
				// funcs
				else if (tokenStream[j].dataType == TokenType::FUNC) {
					//j = std::stoi(tokenStream[j].value);
					//if (j == tokenStream.size() - 1) {
					//	tokenStream.insert(tokenStream.end(), Token(TokenType::RBRACKET));
					//}
					j = findEndOfFunc(tokenStream, j + 1);
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

size_t Parser::findEndOfFunc(std::vector<Token> tokenStream, size_t pos) {
	size_t bracketSur = 1;
	for (size_t i = pos + 1; i < tokenStream.size(); i++) {
		if (tokenStream[i].dataType == TokenType::LBRACKET) {
			bracketSur++;
		}
		else if (tokenStream[i].dataType == TokenType::RBRACKET) {
			bracketSur--;
			if (bracketSur == 0) {
				return i;
			}
		}
	}
	return -1;
}

int Parser::getBrackCount(std::vector<Token> tokStream)
{
	int out = 0;
	for (auto i = tokStream.begin(); i != tokStream.end(); i++) {
		if (i->dataType == TokenType::LBRACKET) {
			out++;
		}
		else if (i->dataType == TokenType::RBRACKET) {
			out--;
		}
	}
	return out;
}
