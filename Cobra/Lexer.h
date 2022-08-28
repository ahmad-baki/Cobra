#pragma once
#include <string_view>
#include <vector>
#include <map>
#include "Token.h"
#include "Error.h"
#include "TokenType.cpp"


struct LexReturn {
	std::vector<Token> tokenString;
	Error error;
};

class Lexer
{
public:
	Lexer(std::string_view text, std::string_view path);
	LexReturn lex();

private:
	std::string_view text;
	std::string_view path;
	size_t pos;
	size_t line;
	size_t column;
	char currentChar;

	void advance();
	void revert(size_t pos);
	std::pair<Token, Error> getNextToken();
	std::pair<Token, Error> getNextNumber();
	std::pair<Token, Error> getNextOperator();
	std::pair<Token, Error> getNextWord();
	bool isInt(int val);

	void skipSpace();
	
	std::map<std::string, enum TokenType> operators{
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
		{"<", TokenType::SMALL},
		{"<=", TokenType::SMALLEQ},
		{">", TokenType::BIG},
		{">=", TokenType::BIGEQ},
	};

	std::map<std::string, enum TokenType> keywords{
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"while", TokenType::WHILE},
		{"int", TokenType::INTWORD},
		{"float", TokenType::FLOATWORD},
	};

	//std::vector<std::string> tokenNames;
	//std::vector<enum TokenType> tokenTypes;
};

