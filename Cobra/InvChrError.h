#pragma once
#include "Error.h"
class InvChrError : public Error
{
public:
	InvChrError(std::string_view desc, size_t line, size_t columnStart, size_t columnEnd, std::string_view path, std::string_view text);
};

