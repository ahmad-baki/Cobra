#pragma once
#include "Error.h"

class RuntimeError : public Error
{
public:
	RuntimeError(std::string_view desc, size_t line = 0, size_t columnStart = 0, size_t columnEnd = 0, std::string_view path = "", std::string_view text = "");
};