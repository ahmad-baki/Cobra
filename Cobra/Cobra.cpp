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
	//std::string input = "int i = 1; while (i != 4) { print(i); i = i + 1;}";
	Lexer lexer = Lexer(input, "<stdin>");
	auto returnLex = lexer.lex();
	if (returnLex.second.m_errorName != "NULL")
		std::cout << returnLex.second;
	else {
		std::vector<Token> tokenStream = returnLex.first;
		Parser parser = Parser();
		BlockNode* blockNode = parser.parse(tokenStream);
		blockNode->run();
	}
	std::cout << "finished" << std::endl;
	std::system("pause");
}

void execFromCommandLine() {
	Parser parser = Parser();
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
		auto returnLex = lexer.lex();
		if (returnLex.second.m_errorName != "NULL")
			std::cout << returnLex.second;
		else {
			std::vector<Token> tokenStream = returnLex.first;
			parser.parse(tokenStream, &blockNode);
			blockNode.contin();
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
}
