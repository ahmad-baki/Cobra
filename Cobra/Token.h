#pragma once
#include <string>
#include "TokenType.h"

class Token {
public:
	enum TokenType type;
	std::string value;
	size_t line;
	size_t startColumn;
	size_t endColumn;


	Token();
	Token(enum TokenType returnType, size_t line = 0, size_t startColumn = 0, size_t endColumn = 0, std::string value = "");
	//Token(enum TokenType returnType, size_t line = 0, size_t column = 0, char value = ' ');
	friend std::ostream& operator<<(std::ostream& output, const Token& e);
};