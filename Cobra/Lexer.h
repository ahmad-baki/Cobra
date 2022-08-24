#pragma once
#include <string_view>
#include <vector>
#include <map>
#include "Token.h"
#include "Error.h"
#include "TokenType.cpp"

class Lexer
{
public:
	Lexer(std::string_view text, std::string_view fileName);
	std::pair<std::vector<Token>, Error> lex();

private:
	std::string_view _text;
	std::string_view _fileName;
	size_t _pos;
	size_t _line;
	size_t _column;
	char _currentChar;

	void advance();
	void revert(size_t pos);
	std::pair<Token, Error> getNextToken();
	std::pair<Token, Error> getNextNumber();
	std::pair<Token, Error> getNextOperator();
	std::pair<Token, Error> getNextWord();
	bool isInt(int val);

	void skipSpace();
	
	std::map<std::string, enum TokenType> operators;
	std::map<std::string, enum TokenType> keywords;
	//std::vector<std::string> tokenNames;
	//std::vector<enum TokenType> tokenTypes;
};

