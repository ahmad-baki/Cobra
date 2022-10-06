#pragma once
#include "Error.h"
class InvChrError : public Error
{
public:
	InvChrError(std::string_view desc, size_t line, size_t startColumn, size_t endColumn, std::string_view path, std::string_view text);
};

