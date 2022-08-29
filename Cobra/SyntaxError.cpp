#include "SyntaxError.h"

SyntaxError::SyntaxError(std::string_view desc, size_t line, size_t columnStart, size_t columnEnd, std::string_view path, std::string_view text)
{
	m_errorName = "SyntaxError";
	this->desc = desc;
	this->path = path;
	this->text = text;
	this->line = line;
	this->columnStart = columnStart;
	this->columnEnd = columnEnd;

}