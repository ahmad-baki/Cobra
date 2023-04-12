#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <regex>
#include "Parser.h"
#include "Lexer.h"
#include "Interpreter.h"
#include "Cobra.h"
#include "SyntaxError.h"
#include "RuntimeError.h"

using namespace std::chrono;

const std::regex STDIMPORTREGEX{"^#import\\s+<.*>"};
const std::regex FILEIMPORTREGEX{ "^#import\\s+\".*\"" };
const std::regex STDIMPORTPATH{ "\\\"(.*)\\\"" };

int main(int argc, char* argv[])
{
	Error outError{};
	if (argc > 1) {
		for (size_t i = 1; i < argc; i++) {
			Interpreter::reset();
			double runTime = execFromFile(argv[i], outError);
			//std::cout << "finished in: " << runTime << "ms" << std::endl;
			//std::cout << std::endl;
		}
	}
	else {
		Interpreter::reset();
		execFromCommandLine(outError);
	}	
	if (outError.errType != ErrorType::NULLERROR) {
		std::cout << outError << std::endl;
	}
	//std::system("pause");
}

std::string trim(std::string input) {
	size_t startPos = 0;
	while (startPos < input.size() && std::isspace(input[startPos])) {
		startPos++;
	}
	size_t endPos = input.size() - 1;

	while (endPos > -1 && std::isspace(input[endPos])) {
		endPos--;
	}
	std::string out = input.substr(startPos, endPos - startPos + 1);
	return out;
}

std::string readFileIntoString(fs::path path, Error& outError) {

	// Sanity check
	if (!std::filesystem::is_regular_file(path)) {
		Error targetError{ ErrorType::RUNTIMEERROR, "dir: " + fs::absolute(path).generic_string() + " not found" };
		outError.copy(targetError);
		return "";
	}

	// Open the file
	// Note that we have to use binary mode as we want to return a string
	// representing matching the bytes of the file on the file system.
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		Error targetError{ ErrorType::RUNTIMEERROR,
			"Error while opening: " + fs::absolute(path).generic_string() };
		outError.copy(targetError);
		return "";
	}

	// Read contents
	std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

	// Close the file
	file.close();

	return content;

	//struct stat info;
	//if (stat(path, &info) == 0) {
	//	std::ifstream t(path);
	//	std::stringstream buffer;
	//	buffer << t.rdbuf();
	//	return buffer.str();
	//}
	//else {

	//}
}

double execFromFile(fs::path path, Error& outError) {
	std::string input = readFileIntoString(path, outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}

	std::filesystem::path absPath = fs::absolute(path);

	std::vector<fs::path> importedFiles{ absPath };
	std::vector<Token> tokenString = getTokenStream(input, path, outError, importedFiles);
	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}


	Parser parser = Parser();
	std::vector<Statement*> statements = parser.parse(tokenString, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}

	//statements.insert(statements.end(), fileStatements.begin(), fileStatements.end());
	Interpreter* interpreter = Interpreter::getSingelton();

	std::vector<Token> stdImports = getSTDImports(tokenString, outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}

	interpreter->importSTD(stdImports, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}

	interpreter->setStatements(statements);

	const auto startTime = system_clock::now();
	interpreter->run(outError);
	const auto endTime = system_clock::now();
	const double runTime = duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (outError.errType != ErrorType::NULLERROR) {
		outError.path = path;
		outError.text = input;
	}
	return runTime;
}

void execFromCommandLine(Error& outError) {
	size_t line = 1;
	Interpreter* interpreter = Interpreter::getSingelton();

	// just for debugging
	interpreter->importSTD({}, outError);
	// !!!!!


	std::vector<fs::path> importedFiles{ };
	while (true) {
		std::cout << "> ";
		std::string input = "";
		getline(std::cin, input);

		std::string tr = trim(input);
		if (trim(input) == "exit") {
			std::cout << "do you want to quit? (y/n)" << std::endl;
			std::cin >> input;
			if (input == "y")
				return;
			continue;
		}

		Lexer lexer = Lexer(input, "<stdin>", line);

		std::vector<Token> tokenString = getTokenStream(input, "<stdin>", outError, importedFiles);

		if (outError.errType != ErrorType::NULLERROR) {
			std::cout << outError << std::endl;
			continue;
		}

		Parser parser = Parser();
		std::vector<Statement*> statements = parser.parse(tokenString, outError);

		if (outError.errType != ErrorType::NULLERROR) {
			std::cout << outError << std::endl;
			continue;
		}

		std::vector<Token> stdImports = getSTDImports(tokenString, outError);

		if (outError.errType != ErrorType::NULLERROR) {
			std::cout << outError << std::endl;
			continue;
		}

		interpreter->importSTD(stdImports, outError);
		if (outError.errType != ErrorType::NULLERROR) {
			std::cout << outError << std::endl;
			continue;
		}

		interpreter->addStatements(statements);
		interpreter->contin(outError);
		if (outError.errType != ErrorType::NULLERROR) {
			outError.path = "<stdin>";
			outError.text = input;
			return;
		}
		line++;
	}
}

std::vector<Token> getTokenStream(std::string_view input, fs::path path, Error& outError, 
	std::vector<fs::path>& importedFiles)
{
	Lexer lexer = Lexer(input, path);
	std::vector<Token> tokenString = lexer.lex(outError);	

	if (outError.errType != ErrorType::NULLERROR) {
		return {};
	}

	auto files = runFileImport(tokenString, importedFiles, outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return {};
	}

	for (auto file = files.begin(); file != files.end(); file++) {
		if (find(importedFiles.begin(), importedFiles.end(), get<1>(*file)) == importedFiles.end()) {
			importedFiles.push_back(get<1>(*file));
		}
	}

	std::vector<Token> out{};
	for (auto file = files.begin(); file != files.end(); file++) {
		std::vector<Token> newTokenString = getTokenStream(get<0>(*file), get<1>(*file), outError, importedFiles);
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
		out.insert(out.end(), newTokenString.begin(), newTokenString.end());
	}
	out.insert(out.end(), tokenString.begin(), tokenString.end());
	return out;
}

std::vector<std::tuple<std::string, std::string>> runFileImport(std::vector<Token>& tokens, 
	std::vector<fs::path> ignoreFiles, Error& outError)
{
	std::vector<Token*> imports{};
	std::smatch base_match;
	for (size_t i = 0; i < tokens.size() && tokens[i].dataType == TokenType::MAKRO; i++) {
		if (std::regex_match(tokens[i].value, base_match, FILEIMPORTREGEX)) {
			imports.push_back(&tokens[i]);
		}
	}
	
	std::vector<std::tuple<std::string, std::string>> out{};
	for (auto iter = imports.begin(); iter != imports.end(); iter++) {

		std::regex_search((*iter)->value, base_match, STDIMPORTPATH);
		std::string file = readFileIntoString(fs::path(base_match[1].str()), outError);

		if (outError.errType != ErrorType::NULLERROR) {
			return{};
		}
	
		out.push_back({ file, (*iter)->value });
	}

	//tokens.erase(std::remove_if(tokens.begin(), tokens.end(), [&](Token x)->bool {
	//	return std::find(out.begin(), out.end(), x) != out.end();
	//}), tokens.end());

	//tokens.erase(remove_if(begin(tokens), end(tokens),
	//	[&](auto x) {return find(begin(imports), end(imports), x) != end(imports); }), end(tokens));

	auto it = remove_if(tokens.begin(), tokens.end(), [&imports](const Token& token) {
		return find(imports.begin(), imports.end(), &token) != imports.end();
	});

	tokens.erase(it, tokens.end());

	return out;
}


std::vector<Token> getSTDImports(std::vector<Token>& tokens, Error& outError)
{
	// finds right tokens
	std::vector<Token> out{};
	std::smatch base_match;
	for (size_t i = 0; i < tokens.size() && tokens[i].dataType == TokenType::MAKRO; i++) {
		if (std::regex_match(tokens[i].value, base_match, STDIMPORTREGEX)) {
			out.push_back(tokens[i]);

			std::regex_search(tokens[i].value, base_match, STDIMPORTPATH);
			tokens[i].value = base_match[1].str();
		}
		//else if (std::regex_match(tokens[i].value, base_match, CUSTIMPORTREGEX)) {
		//	remove.push_back(tokens[i]);
		//}
	}

	// remove imports from tokenstring
	for (auto iter = out.begin(); iter != out.end(); iter++) {
		tokens.erase(iter);

	}
	return out;
}