#pragma once
#include "Error.h"

class SyntaxError : public Error
{
public:
	SyntaxError(std::string_view desc, std::string_view path, std::string_view text, size_t line, size_t columnStart, size_t columnEnd);
};