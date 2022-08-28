#include "Lexer.h"
#include "Error.h"
#include "InvChrError.h"
#include <list>


Lexer::Lexer(std::string_view text, std::string_view path) : 
	text{ text }, path{ path }, pos{ 0 }, line{1}, column{0}, currentChar{' '}
{	
	if (text.size() > 0)
		currentChar = text[0];
}

LexReturn Lexer::lex()
{
	std::vector<Token> tokens{};
	while (currentChar != 0) {
		auto [token, error] = getNextToken();

		// when lexer throws error
		if (error.m_errorName != "NULL") {
			return { {}, error};
		}
		tokens.push_back(token);
		skipSpace();
	}
	return {tokens, Error()};
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

std::pair<Token, Error> Lexer::getNextToken() {

	while (isspace(currentChar))
		advance();

	// 1. return literal
	if (isInt(currentChar)) {
		return getNextNumber();
	}

	// 2. return operator
	 auto lexReturn = getNextOperator();
	 if (lexReturn.second.m_errorName == "NULL") {
		 return lexReturn;
	 }

	// 3. return identifier or keyword
	return getNextWord();
}

std::pair<Token, Error> Lexer::getNextNumber() {
	std::string tokenString{ "" };
	bool containsPeriod = false;
	while (currentChar != 0 && (isInt(currentChar) || currentChar == '.')) {
		if (currentChar == '.') {
			if (containsPeriod) {
				return { Token(), 
					InvChrError("More than one '.' in decimal", path,  text, line, column, column+1)
				};
			}
			containsPeriod = true;
		}
		tokenString += currentChar;
		advance();
	}
	size_t startColumn = column + 1 - tokenString.size();
	size_t endcolumn = column + 1;

	if (containsPeriod) {
		return { Token(TokenType::DECLIT, line, startColumn, endcolumn, tokenString), 
			Error()
		};
	}
	return { Token(TokenType::INTLIT, line, startColumn, endcolumn, tokenString), 
		Error()
	};
}

std::pair<Token, Error> Lexer::getNextOperator() 
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
	while (possNextTokens.size() > 1) {
		opString += currentChar;
		advance();

		if (isspace(currentChar) || currentChar == 0) {
			auto elem = operators.find(opString);
			if (elem != operators.end()) {
				size_t startColumn = column + 1- opString.size();
				size_t endColumn = column + 1;
				return {Token(elem->second, line, startColumn, endColumn), Error()};
			}
			return { Token(), 
				InvChrError("Received Invalied Token", path, text, line, column, column+1)
			};
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
				return { Token(operators[opString], line, startColumn, endColumn), 
					Error()
				};
			}
		}
		return { Token(), 
			InvChrError("Received Invalied Token: " + currentChar, path, text, line, column, column+1)
		};
	}

	// when there is only one poss.
	opString += currentChar;
	size_t startColumn = column + 1 - opString.size();
	size_t endColumn = column + 1;
	advance();
	return { Token(operators[opString], line, startColumn, endColumn),
		Error()
	};
}

std::pair<Token, Error> Lexer::getNextWord() {
	std::string tokenString{ "" };
	while (isalpha(currentChar) || isInt(currentChar) || currentChar == '_') {
		tokenString += currentChar;
		advance();
	}
	auto elem = keywords.find(tokenString);
	if (elem == keywords.end()) {
		if (tokenString.size() == 0) {
			return { Token(),
			InvChrError("Received Invalied Token", path, text, line, column, column + 1)
			};
		}


		size_t startColumn = column + 1 - tokenString.size();
		size_t endColumn = column + 1;
		return { Token(TokenType::IDENTIFIER, line, startColumn, endColumn, tokenString), 
			Error()
		};
	}
	size_t startColumn = column + 1 - tokenString.size();
	size_t endColumn = column + 1;
	return { Token(elem->second, line, startColumn, endColumn, tokenString), 
		Error() 
	};
}

bool Lexer::isInt(int val) {
	return 47 < val &&  val < 58;
}

void Lexer::skipSpace() {
	while (isspace(currentChar))
		advance();
}