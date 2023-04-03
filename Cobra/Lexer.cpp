#include "Lexer.h"
#include "Error.h"
#include "InvChrError.h"


Lexer::Lexer(std::string_view text, fs::path path, size_t line) : 
	text{ text }, path{ path }, pos{ 0 }, line{line}, column{0}, currentChar{' '}
{	
	if (text.size() > 0)
		currentChar = text[0];
}

std::vector<Token> Lexer::lex(Error& outError)
{
	std::vector<Token> tokens{};
	skipSpace();
	while (currentChar != 0) {
		Token token = getNextToken(outError);

		// when lexer throws error
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
		tokens.push_back(token);
		skipSpace();
	}
	return tokens;
}

void Lexer::advance()
{
	pos++;
	if (pos < text.size()) {
		if (currentChar == '\n') {
			line++;
			column = 0;
		}
		else {
			column++;
		}
		currentChar = text[pos];
	}
	else {
		currentChar = 0;
	}
}

void Lexer::revert(size_t pos)
{
	// calc correct line
	for (size_t i = pos; i < pos; i++) {
		if (text[i] == '\n') {
			line--;
		}
	}

	bool foundSol = false;
	// calc correct column
	for (size_t i = 1; i < pos - 1; i++) {
		if (text[pos - i] == '\n') {
			column = i - 1;
			foundSol = true;
		}
	}
	if (!foundSol) {
		column = 0;
	}
}

Token Lexer::getNextToken(Error& outError) {

	while (isspace(currentChar))
		advance();

	// 1. return makro operator
	if (currentChar == '#') {
		return getNextMakro(outError);
	}

	// 2. return char-literal
	if (currentChar == '\'') {
		return getNextChar(outError);
	}

	// 3. return number-literal
	if (isInt(currentChar)) {
		return getNextNumber(outError);
	}

	// 4. return operator
	// check how this works !!!!!!!!!!!!!!!!
	Error opError = Error();
	Token token = getNextOperator(opError);
	if (token.dataType != TokenType::NONE) {
		return token;
	}

	// 5. return identifier or keyword
	return getNextWord(outError);
}

Token Lexer::getNextMakro(Error& outError)
{
	size_t startPos = pos;
	while (currentChar != '\n') {
		advance();
	}
	Token out{ TokenType::MAKRO, path, text, line, startPos, pos,
		std::string(text.substr(startPos, pos - startPos)) };
	advance();
	return out;
}

Token Lexer::getNextChar(Error& outError) {
	size_t startColumn = column;
	advance();
	std::string tokenString{ currentChar };
	advance();
	if (currentChar != '\'') {
		Error copyTarget(ErrorType::INVCHRERROR, "Missing \'", line, column, column + 1, path, text);
		outError.copy(copyTarget);
		return Token();
	}
	size_t endColumn = column;
	advance();
	return Token(TokenType::CHARLIT, path, text, line, startColumn, endColumn, tokenString);
}

Token Lexer::getNextNumber(Error& outError) {
	std::string tokenString{ "" };
	bool containsPeriod = false;
	while (currentChar != 0 && (isInt(currentChar) || currentChar == '.')) {
		if (currentChar == '.') {
			if (containsPeriod)
			{
				Error copyTarget(ErrorType::INVCHRERROR, "More than one '.' in decimal", line, column, column + 1, path, text);
				outError.copy(copyTarget);
				return Token();
			}
			containsPeriod = true;
		}
		tokenString += currentChar;
		advance();
	}
	size_t startColumn = column + 1 - tokenString.size();
	size_t endcolumn = column + 1;

	if (containsPeriod) {
		return Token(TokenType::DECLIT, path, text, line, startColumn, endcolumn, tokenString);
	}
	return Token(TokenType::INTLIT, path, text, line, startColumn, endcolumn, tokenString);
}

Token Lexer::getNextOperator(Error& outError) 
{
	// 1. finds all operators which could match current char
	std::list<std::string> possNextTokens{};
	for (auto token : operators) {
		if (token.first[0] == currentChar)
			possNextTokens.push_back(token.first);
	}

	// 2. advances as long there are mult poss. tokens, and it completly parsed tokenstring
	int i = 1;
	std::string opString{  };
	while (possNextTokens.size() != 0 && i < getMaxLen(possNextTokens)) {
		opString += currentChar;
		advance();

		if (isspace(currentChar) || currentChar == 0) {
			auto elem = operators.find(opString);
			if (elem != operators.end()) {
				size_t startColumn = column + 1- opString.size();
				size_t endColumn = column + 1;
				return Token(elem->second, path, text, line, startColumn, endColumn);
			}
			Error copyTarget(ErrorType::INVCHRERROR, "Received Invalied Token", line, column, 
				column + 1, path, text);
			outError.copy(copyTarget);
			return Token();
		}

		// removes operators which are not poss.
		std::list<std::string> newPossTokens{};
		for (auto tokenString : possNextTokens){
			if (tokenString.size() > i || currentChar == tokenString[i]){
				newPossTokens.push_back(tokenString);
			}
		}
		possNextTokens = newPossTokens;
		i++;
	}

	if (possNextTokens.size() == 0) {
		if (opString.size() > 0) {
			auto elem = operators.find(opString);
			if (elem != operators.find(opString)) {
				size_t startColumn = column + 1 - opString.size();
				size_t endColumn = column + 1;
				return Token(operators[opString], path, text, line, startColumn, endColumn);
			}
		}
		Error copyTarget(ErrorType::INVCHRERROR, "Received Invalied Token: " + currentChar, 
			line, column, column + 1, path, text);
		outError.copy(copyTarget);
		return Token();
	}

	// when there is only one poss.
	opString += currentChar;
	size_t startColumn = column + 1 - opString.size();
	size_t endColumn = column + 1;
	advance();
	return Token(operators[opString], path, text, line, startColumn, endColumn);
}

Token Lexer::getNextWord(Error& outError) {
	std::string tokenString{ "" };
	while (isalpha(currentChar) || isInt(currentChar) || currentChar == '_') {
		tokenString += currentChar;
		advance();
	}
	auto elem = keywords.find(tokenString);
	if (elem == keywords.end()) {
		if (tokenString.size() == 0) {
			Error copyTarget(ErrorType::INVCHRERROR, "Received Invalied Token", line, column, 
				column + 1, path, text);
			outError.copy(copyTarget);
			return Token();
		}


		size_t startColumn = column + 1 - tokenString.size();
		size_t endColumn = column + 1;
		return Token(TokenType::IDENTIFIER, path, text, line, startColumn, endColumn, tokenString);
	}
	size_t startColumn = column + 1 - tokenString.size();
	size_t endColumn = column + 1;
	return Token(elem->second, path, text, line, startColumn, endColumn, tokenString);
}

bool Lexer::isInt(int val) {
	return 47 < val &&  val < 58;
}

void Lexer::skipSpace() {
	while (isspace(currentChar))
		advance();
}

size_t Lexer::getMaxLen(std::list<std::string> input) {
	size_t maxLen = 0;
	for (std::string str : input) {
		if (str.size() > maxLen) {
			maxLen = str.size();
		}
	}
	return maxLen;
}