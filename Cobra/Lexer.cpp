#include "Lexer.h"
#include "Error.h"
#include <list>


Lexer::Lexer(std::string_view text, std::string_view fileName) : 
	_text{ text }, _fileName{ fileName }, _pos{ 0 }, _currentChar{ text[0]}
{
	operators = std::map<std::string, TokenType> {
		{"+", TokenType::PLUS},
		{"-", TokenType::MINUS},
		{"*", TokenType::MUL},
		{"/", TokenType::DIV},
		{"(", TokenType::LBRACKET},
		{")", TokenType::RBRACKET},
		{"{", TokenType::LCURLBRACKET},
		{"}", TokenType::RCURLBRACKET},
		{"!", TokenType::EXCLA},
		{"=", TokenType::EQ},
		{"==", TokenType::EQEQ},
		{"!=", TokenType::EXCLAEQ},
		{";", TokenType::SEMICOLON},
	};

	keywords = {
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"while", TokenType::WHILE},
		{"int", TokenType::INTWORD},
		{"float", TokenType::FLOATWORD},
	};
}

std::pair<std::vector<Token>, Error> Lexer::lex()
{
	std::vector<Token> operators{};
	while (_currentChar != 0) {
		std::pair<Token, Error> returnValues = getNextToken();

		// when lexer throws error
		if (returnValues.second.m_errorName != "NULL") {
			return std::pair{ operators, returnValues.second };
		}
		operators.push_back(returnValues.first);
		skipSpace();
	}
	return std::pair<std::vector<Token>, Error>{operators, Error()};
}

void Lexer::advance()
{
	_pos++;
	if (_pos < _text.size()) {
		_currentChar = _text[_pos];
	}
	else {
		_currentChar = 0;
	}
}

void Lexer::revert(size_t pos)
{
	_pos = pos-1;
	advance();
}

std::pair<Token, Error> Lexer::getNextToken() {

	while (isspace(_currentChar))
		advance();

	// 1. return literal
	if (isInt(_currentChar)) {
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
	while (_currentChar != 0 && (isInt(_currentChar) || _currentChar == '.')) {
		if (_currentChar == '.') {
			if (containsPeriod) {
				return std::pair<Token, Error>{
					Token(TokenType::NONE), Error("IvalidCharacterError", "More than one '.' in decimal", _text, _line, _column)
				};
			}
			containsPeriod = true;
		}
		tokenString += _currentChar;
		advance();
	}
	if (containsPeriod) {
		return std::pair<Token, Error>{
			Token(TokenType::DECLIT, tokenString), Error()
		};
	}
	return std::pair<Token, Error>{
		Token(TokenType::INTLIT, tokenString), Error()
	};
}

std::pair<Token, Error> Lexer::getNextOperator() 
{
	// 1. finds all operators which could match current char
	std::list<std::string> possNextTokens{};
	for (auto token : operators) {
		if (token.first[0] == _currentChar)
			possNextTokens.push_back(token.first);
	}

	// 2. advances as long there are mult poss. tokens, and it completly parsed tokenstring
	int i = 1;
	std::string opString{ "" };
	while (possNextTokens.size() > 1) {
		opString += _currentChar;
		advance();

		if (isspace(_currentChar) || _currentChar == 0) {
			auto elem = operators.find(opString);
			if (elem != operators.end()) {
				return std::pair<Token, Error>{Token(elem->second), Error()};
			}
			return std::pair<Token, Error>{
				Token(TokenType::NONE), Error("IvalidCharacterError", "Received Invalied Token", _text, _line, _column)
			};
		}

		// removes operators which are not poss.
		std::list<std::string> newPossTokens{};
		for (auto tokenString : possNextTokens){
			if (tokenString.size() > i || _currentChar == tokenString[i]){
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
				return std::pair<Token, Error>{Token(operators[opString]), Error()};
			}
		}
		return std::pair<Token, Error>{
			Token(TokenType::NONE), Error("IvalidCharacterError", "Received Invalied Token", _text, _line, _column)
		};
	}

	// when there is only one poss.
	advance();
	return std::pair<Token, Error>{
		Token(operators[possNextTokens.front()]), Error()
	};
}

std::pair<Token, Error> Lexer::getNextWord() {
	std::string tokenString{ "" };
	while (isalpha(_currentChar) || isInt(_currentChar) || _currentChar == '_') {
		tokenString += _currentChar;
		advance();
	}
	auto elem = keywords.find(tokenString);
	if (elem == keywords.end()) {
		return std::pair<Token, Error>{Token(TokenType::IDENTIFIER, tokenString), Error()};
	}
	return std::pair<Token, Error>{Token(elem->second), Error()};
}

bool Lexer::isInt(int val) {
	return 47 < val &&  val < 58;
}

void Lexer::skipSpace() {
	while (isspace(_currentChar))
		advance();
}