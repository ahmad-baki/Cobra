#pragma once
#include <string_view>
#include <iostream>
#include <optional>


class Error
{
public:
	friend std::ostream& operator<<(std::ostream& output, const Error& e);
	Error();
	Error(std::string_view errorName, std::string_view desc, std::string_view text, size_t line, size_t column);

	std::string m_errorName;

private:
	std::string _desc;
	std::string_view _text;
	std::string errorLine;
	size_t _lineIndex;
	size_t _columnIndex;

	std::string getErrorLine();
};

