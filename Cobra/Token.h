#pragma once
#include <string>
#include "TokenType.h"

class Token {
public:
	enum TokenType _type;
	std::string _value;

	Token(enum TokenType returnType, std::string value = "");
	Token(enum TokenType returnType, char value);
	friend std::ostream& operator<<(std::ostream& output, const Token& e);

private:
};