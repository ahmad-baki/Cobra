#pragma once
#include <list>
#include <string_view>
#include <vector>
#include <map>
#include "Token.h"
#include "Error.h"
#include "TokenType.cpp"



class Lexer
{
public:
	Lexer(std::string_view text, std::string_view path, size_t line = 1);
	std::vector<Token> lex(Error& outError);

private:
	std::string_view text;
	std::string_view path;
	size_t pos;
	size_t line;
	size_t column;
	char currentChar;

	void advance();
	void revert(size_t pos);
	Token getNextToken(Error& outError);
	Token getNextNumber(Error& outError);
	Token getNextOperator(Error& outError);
	Token getNextWord(Error& outError);
	bool isInt(int val);

	void skipSpace();

	size_t getMaxLen(std::list<std::string> input);
	
	std::map<std::string, enum TokenType> operators{
		{"+",		TokenType::PLUS},
		{"-",		TokenType::MINUS},
		{"*",		TokenType::MUL},
		{"/",		TokenType::DIV},
		{"(",		TokenType::LBRACKET},
		{")",		TokenType::RBRACKET},
		{"[",		TokenType::LSQBRACKET},
		{"]",		TokenType::RSQBRACKET},
		{"{",		TokenType::LCURLBRACKET},
		{"}",		TokenType::RCURLBRACKET},
		{"!",		TokenType::EXCLA},
		{"=",		TokenType::EQ},
		{"==",		TokenType::EQEQ},
		{"!=",		TokenType::EXCLAEQ},
		{";",		TokenType::SEMICOLON},
		{"<",		TokenType::SMALL},
		{"<=",		TokenType::SMALLEQ},
		{">",		TokenType::BIG},
		{">=",		TokenType::BIGEQ},
		{"&&",		TokenType::AND},
		{"||",		TokenType::OR},
		{"%",		TokenType::MOD},
	};

	std::map<std::string, enum TokenType> keywords{
		{"if",		TokenType::IF},
		{"else",	TokenType::ELSE},
		{"while",	TokenType::WHILE},
		{"int",		TokenType::INTWORD},
		{"float",	TokenType::FLOATWORD},
		{"var",		TokenType::VARWORD},
		{"const",	TokenType::CONSTWORD},
	};

	//std::vector<std::string> tokenNames;
	//std::vector<enum TokenType> tokenTypes;
};

