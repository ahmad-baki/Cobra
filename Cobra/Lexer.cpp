#include "Lexer.h"
#include "Error.h"


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
		{";", TokenType::SEMICOLON}
	};
}

std::pair<std::vector<Token>, Error> Lexer::lex()
{
	std::vector<Token> operators{};
	while (_currentChar != NULL) {
		std::pair<Token, Error> returnValues = getNextToken();

		// when lexer throws error
		if (returnValues.second.m_errorName != "NULL") {
			return std::pair{ operators, returnValues.second };
		}
		operators.push_back(returnValues.first);
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
		_currentChar = NULL;
	}
}

std::pair<Token, Error> Lexer::getNextToken() {

	while (isspace(_currentChar))
		advance();

	// 1. return literal
	if (isInt(_currentChar)) {
		return getNextNumber();
	}

	// 2. return operator
	if (isOperator(_currentChar)) {
		return getNextOperator();
	}

	// 3. return identifier or keyword
	return getNextWord();
}

std::pair<Token, Error> Lexer::getNextNumber() {
	std::string tokenString{ "" };
	bool containsPeriod = false;
	while (_currentChar != NULL && (isInt(_currentChar) || _currentChar == '.')) {
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

std::pair<Token, Error> Lexer::getNextOperator() {

	// 1. finds all operators which could match current char
	std::vector<std::string> possNextTokens{};
	for (auto token : operators) {
		if (token.first[0] == _currentChar)
			possNextTokens.push_back(token.first);
	}

	// 2. advances as long there are mult poss. tokens
	int i = 0;
	std::string currentTokenName{ _currentChar };
	while (possNextTokens.size() > 1) {
		advance();
		i++;

		if (isspace(_currentChar) || _currentChar == NULL) {
			auto elem = operators.find(currentTokenName);
			if (elem != operators.end()) {
				return std::pair<Token, Error>{Token(elem->second), Error()};
			}
			return std::pair<Token, Error>{
				Token(TokenType::NONE), Error("IvalidCharacterError", "Received Invalied Token", _text, _line, _column)
			};
		}

		// removes operators which are not poss.
		std::vector<size_t> delList{};
		for (size_t j = 0; j < possNextTokens.size(); j++) {
			std::string token = possNextTokens[j];
			if (_currentChar != token[i])
				delList.push_back(j);
		}
		for (int j = 0; j < delList.size(); j++)
		{
			possNextTokens.erase(possNextTokens.begin() + delList[j]-j);
		}
	}

	if (possNextTokens.size() == 0) {
		auto elem = operators.find(currentTokenName);
		if (elem != operators.find(currentTokenName)) {
			return std::pair<Token, Error>{Token(operators[currentTokenName]), Error()};
		}
		return std::pair<Token, Error>{
			Token(TokenType::NONE), Error("IvalidCharacterError", "Received Invalied Token", _text, _line, _column)
		};
	}

	// when there is only one poss.
	advance();
	return std::pair<Token, Error>{
		Token(operators[possNextTokens[0]]), Error()
	};
}

std::pair<Token, Error> Lexer::getNextWord() {
	std::string tokenString{ "" };
	while (!(isspace(_currentChar) || isOperator(_currentChar) || _currentChar == NULL)) {
		tokenString += _currentChar;
		advance();
	}
	auto elem = keywords.find(tokenString);
	if (elem == keywords.end()) {
		return std::pair<Token, Error>{Token(TokenType::IDENTIFIER, tokenString), Error()};
	}
	return std::pair<Token, Error>{Token(elem->second), Error()};
}

bool Lexer::isOperator(char chr) {
	std::vector<std::string> possNextTokens{};
	for (auto token : operators) {
		if (token.first[0] == _currentChar)
			return true;
	}
	return false;
}

bool Lexer::isInt(int val) {
	return 47 < val &&  val < 58;
}