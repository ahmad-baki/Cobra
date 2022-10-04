#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Parser.h"
#include "Lexer.h"
#include <cstdlib>
#include <chrono>

using namespace std::chrono;

std::string readFileIntoString(std::string path) {
	struct stat info;
	if (stat(path.c_str(), &info) == 0) {
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}
	else {
		std::cout << "dir: " << path << " not found" << std::endl;
		exit(-1);
	}
}

double execFromFile(std::string path) {
	std::string input = readFileIntoString(path);

	Lexer lexer = Lexer(input, path);
	auto [tokenString, lexError] = lexer.lex();

	if (lexError.m_errorName != "NULL"){
		std::cout << lexError << '\n' << std::endl;
		return 0;
	}

	Parser parser = Parser(input, path);
	auto [blockNode, parseError] = parser.parse(tokenString);

	if (parseError.m_errorName != "NULL") {
		std::cout << parseError << '\n' << std::endl;
		return 0;
	}

	const auto startTime = system_clock::now();
	Error runtimeError = blockNode->run();
	const auto endTime = system_clock::now();
	const double runTime = duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (runtimeError.m_errorName != "NULL") {
		runtimeError.path = path;
		runtimeError.text = input;
		std::cout << runtimeError << '\n' << std::endl;
	}
	return runTime;
}

void execFromCommandLine() {
	BlockNode blockNode = BlockNode();
	size_t line = 1;
	while (true) {
		std::cout << "> ";
		std::string input = "";
		getline(std::cin, input);

		if (input == "exit") {
			std::cout << "do you want to quit? (y/n)" << std::endl;
			std::cin >> input;
			if (input == "y")
				return;
			continue;
		}

		Lexer lexer = Lexer(input, "<stdin>", line);
		auto [tokenString, lexError] = lexer.lex();
		if (lexError.m_errorName != "NULL"){
			std::cout << lexError << '\n' << std::endl;
			return;
		}

		Parser parser = Parser(input, "<stdin>");
		Error parseError = parser.parse(tokenString, &blockNode);

		if (parseError.m_errorName != "NULL") {
			std::cout << parseError << '\n' << std::endl;
			return;
		}

		Error runtimeError = blockNode.contin();
		if (runtimeError.m_errorName != "NULL") {
			runtimeError.path = "<stdin>";
			runtimeError.text = input;
			std::cout << runtimeError << '\n' << std::endl;
			return;
		}
		line++;
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		double runTime = execFromFile(argv[1]);
		std::cout << "finished in: " << runTime << "ms" << std::endl;
	}
	else {
		execFromCommandLine();
		std::cout << "finished" << std::endl;
	}	
	std::system("pause");
}
