#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "Parser.h"
#include "Lexer.h"
#include <cstdlib>

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

void execFromFile(std::string path) {
	std::string input = readFileIntoString(path);
	Lexer lexer = Lexer(input, path);
	auto [tokenString, lexError] = lexer.lex();

	if (lexError.m_errorName != "NULL"){
		std::cout << lexError << '\n' << std::endl;
		return;
	}

	Parser parser = Parser(input, path);
	auto [blockNode, parseError] = parser.parse(tokenString);

	if (parseError.m_errorName != "NULL") {
		std::cout << parseError << '\n' << std::endl;
		return;
	}

	Error runtimeError = blockNode->run();
	if (runtimeError.m_errorName != "NULL") {
		std::cout << runtimeError << '\n' << std::endl;
	}
}

void execFromCommandLine() {
	BlockNode blockNode = BlockNode();
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

		Lexer lexer = Lexer(input, "<stdin>");
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
			std::cout << runtimeError << '\n' << std::endl;
			return;
		}
	}
}

int main(int argc, char* argv[])
{	
	if (argc > 1) {
		execFromFile(argv[1]);
	}
	else {
		execFromCommandLine();
	}	
	std::cout << "finished" << std::endl;
	std::system("pause");
}
