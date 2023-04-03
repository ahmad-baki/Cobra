#pragma once
#include <string>
#include "TokenType.h"
#include <filesystem>

namespace fs = std::filesystem;


class Token {
public:
	enum TokenType dataType;
	std::string value;
	size_t line;
	size_t startColumn;
	size_t endColumn;
	fs::path path;
	std::string_view text;

	Token();
	Token(enum TokenType returnType, fs::path path = "", std::string_view text = "", 
		size_t line = 0, size_t startColumn = 0,
		size_t endColumn = 0, std::string value = "");
	//Token(enum TokenType returnType, size_t line = 0, size_t column = 0, char value = ' ');
	friend std::ostream& operator<<(std::ostream& output, const Token& e);
};