#include "Error.h"


std::ostream& operator<<(std::ostream& output, const Error& e)
{
	output << e.m_errorName << ": " << e._desc << std::endl
		<< "at line: " << e._lineIndex << "column: " << e._columnIndex << std::endl
		<< e.errorLine << std::endl;
	return output;
}

Error::Error() : m_errorName{"NULL"}
{}

Error::Error(std::string_view errorName, std::string_view desc, std::string_view text, size_t line, size_t column) :
	m_errorName{ errorName }, _desc{ desc }, _text{ text }, _lineIndex{ line }, _columnIndex{ column }
{
	errorLine = getErrorLine();
}

std::string Error::getErrorLine() {
	size_t lineCount{ 0 };
	size_t currentPos{ 0 };
	while (lineCount < _lineIndex) {
		for (char ch : _text) {
			if (ch == '\n')
				lineCount++;
			currentPos++;
		}
	}
	std::string line{};
	for (char ch : _text.substr(currentPos)) {
		if (ch == '\n')
			break;
		line += ch;
	}
	return line;
}
