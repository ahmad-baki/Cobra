#pragma once
#include <string_view>
#include <iostream>
#include <optional>


class Error
{
public:
	friend std::ostream& operator<<(std::ostream& output, const Error& e);
	Error();
	Error(std::string_view errorName, std::string_view desc, std::string_view path, std::string_view text, size_t line, size_t columnStart, size_t columnEnd);

	std::string m_errorName;

protected:
	std::string desc;
	std::string_view path;
	std::string_view text;
	//std::string errorLine;
	size_t line;
	size_t columnStart;
	size_t columnEnd;

	std::string getErrorLine() const;
	std::string getErrorPointerStr(size_t line) const;
};

