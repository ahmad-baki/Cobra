#include "Error.h"
#include<string>  

std::ostream& operator<<(std::ostream& output, const Error& e)
{
	output
		<< "File: " << e.path << std::endl
		<< e.m_errorName << " (line: " << e.line << ", column: " << e.columnStart << "): "
		<< e.desc << std::endl 
		<< e.getErrorLine();
	return output;
}

Error::Error() 
	: m_errorName{"NULL"}, desc{""}, text{""}, line{0}, columnStart{0}, columnEnd{0}
{}

Error::Error(std::string_view errorName, std::string_view text, size_t line, size_t columnStart, size_t columnEnd, std::string_view desc, std::string_view path) :
	m_errorName{ errorName }, desc{ desc }, path{ path }, text{text}, line{line}, columnStart{columnStart}, columnEnd{columnEnd}
{
}

std::string Error::getErrorLine() const{
	// if first line
	size_t lineBreakCount = 0;
	size_t endStrIndex = text.size()-1;
	std::string outString{ "" };
	if (line == 1) {
		for (size_t i = 0; i < text.size(); i++) {
			if (text[i] == '\n') {
				endStrIndex = i;
				break;
			}
		}
		std::string subStr = std::string(text.substr(0, endStrIndex));
		outString = "1:\t" + subStr + '\n' + getErrorPointerStr(subStr);

		for (size_t i = endStrIndex + 1; i < text.size(); i++) {
			if (text[i] == '\n') {
				outString += "\n2:\t" + std::string(text.substr(endStrIndex + 1, i - (endStrIndex + 1)));
				break;
			}
		}
	}
	else {
		size_t startStrIndex = 0;
		bool foundEnd = false;
		for (size_t i = 0; i < text.size(); i++) {
			if (text[i] == '\n') {
				lineBreakCount++;
				if (lineBreakCount == line - 2) {
					startStrIndex = i + 1;
				}
				else if (lineBreakCount == line - 1) {
					outString += std::to_string(line - 1) + ":\t" 
						+ std::string(text.substr(startStrIndex, i - startStrIndex)) + '\n';
					startStrIndex = i + 1;
				}
				else if (lineBreakCount == line) {
					std::string errorLine = std::string(text.substr(startStrIndex, i - startStrIndex));
					std::string errorPointer = getErrorPointerStr(errorLine);
					outString += std::to_string(line) + ":\t" + errorLine + '\n' + errorPointer;
					endStrIndex = i + 1;
				}
				else if (lineBreakCount == line + 1) {
					outString += '\n' + std::to_string(line + 1) + ":\t" + std::string(text.substr(endStrIndex, i - endStrIndex));
					foundEnd = true;
				}
			}
		}
		if (!foundEnd) {
			outString += '\n' + std::to_string(line + 1) + ":\t" + std::string(text.substr(endStrIndex));
		}
	}
	return outString;
}

std::string Error::getErrorPointerStr(std::string errorLine) const {
	int digits = std::to_string(line).size();
	// for lineNumber + :, for example: "3: ..."
	std::string output(digits+1, ' ');
	output += std::string(getNTabs(errorLine) + 1,'\t');
	output += std::string(columnStart-1, ' ');
	output += std::string(columnEnd-columnStart, '^');
	return output;
}

size_t Error::getNTabs(std::string errorLine) const
{
	size_t count{ 0 };
	for (size_t i = 0; i < columnStart; i++) {
		if (errorLine[i] == '\t') {
			count++;
		}
	}
	return count;
}
