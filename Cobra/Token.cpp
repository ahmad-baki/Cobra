#include<iostream>
#include "Token.h"
#include "TokenType.cpp"

Token::Token()
	: type{ TokenType::NONE }, value{ "" }, line{ 0 }, startColumn{ 0 }, endColumn{ 0 }
{
}

Token::Token(enum TokenType returnType, size_t line, size_t startColumn, size_t endColumn, std::string value)
	: type{ returnType }, value{ value }, line{ line }, startColumn{ startColumn }, endColumn{ endColumn }
{
}

//Token::Token(enum TokenType returnType, size_t line, size_t column, char value)
//	: type{ returnType }, value{ value }, line{ line }, column{ column }
//{
//}

std::ostream& operator<<(std::ostream& output, const Token& e)
{
	const std::string nameTT[] = { "NONE", "PLUS", "MINUS", "MUL", "DIV", "LBRACKET", "RBRACKET", "LCURLBRACKET", "RCURLBRACKET", "EQ", "EQEQ", "INTLIT", "DECLIT", "EXCLA", "IDENTIFIER"};
	std::string typeName = nameTT[e.type];
	output << "Type: " << typeName << ", Value: " << e.value << std::endl;
	return output;
}
