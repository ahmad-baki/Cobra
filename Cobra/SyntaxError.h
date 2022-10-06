#pragma once
#include "Error.h"

class SyntaxError : public Error
{
public:
	SyntaxError(std::string_view desc, size_t line = 0, size_t startColumn = 0, size_t endColumn = 0, std::string_view path = "", std::string_view text = "");
};