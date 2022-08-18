#include "Token.h"
#include<iostream>

Token::Token(enum TokenType returnType, std::string value) : _type{returnType}, _value{value}
{
}

Token::Token(enum TokenType returnType, char value) : _type{ returnType }, _value{ value }
{
}

std::ostream& operator<<(std::ostream& output, const Token& e)
{
	const std::string nameTT[] = { "NONE", "PLUS", "MINUS", "MUL", "DIV", "LBRACKET", "RBRACKET", "LCURLBRACKET", "RCURLBRACKET", "EQ", "EQEQ", "INTLIT", "DECLIT", "EXCLA", "IDENTIFIER"};
	std::string typeName = nameTT[e._type];
	output << "Type: " << typeName << ", Value: " << e._value << std::endl;
	return output;
}
