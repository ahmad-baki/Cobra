#pragma once
#include <string_view>
#include <iostream>
#include <optional>


class Error
{
public:
	Error();
	Error(std::string_view errorName, std::string_view desc, size_t line = 0, size_t columnStart = 0, size_t columnEnd = 0, std::string_view path = "", std::string_view text = "");

	friend std::ostream& operator<<(std::ostream& output, const Error& e);
	std::string m_errorName;
	std::string desc;
	std::string_view path;
	std::string_view text;
	//std::string errorLine;
	size_t line;
	size_t columnStart;
	size_t columnEnd;

	std::string getErrorLine() const;
	std::string getErrorPointerStr(std::string errorLine) const;
	size_t getNTabs(std::string errorLine) const;
};

