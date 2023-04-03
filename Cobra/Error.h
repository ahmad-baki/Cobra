#pragma once
#include <string_view>
#include <iostream>
#include <optional>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;


class Error
{
private:
	static std::map<enum ErrorType, std::string> errorNames;

public:


	Error();
	Error(ErrorType errTyp, std::string_view desc, size_t line = 0, 
		size_t startColumn = 0, size_t endColumn = 0, fs::path path = {}, std::string_view text = "");

	friend std::ostream& operator<<(std::ostream& output, const Error& e);
	enum ErrorType errType;
	std::string desc;
	fs::path path;
	std::string_view text;
	//std::string errorLine;
	size_t line;
	size_t startColumn;
	size_t endColumn;

	void copy(Error& outError);
	std::string getErrorLine() const;
	std::string getErrorPointerStr(std::string errorLine) const;
	size_t getNTabs(std::string errorLine) const;
};

enum ErrorType {
	NULLERROR, INVCHRERROR, SYNTAXERROR, RUNTIMEERROR,
};
