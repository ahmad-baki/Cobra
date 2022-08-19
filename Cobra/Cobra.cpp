#include <iostream>
#include <windows.h>
#include <string>
#include <fstream>
#include <sstream>
#include "Parser.h"
#include "Lexer.h"

std::string readFileIntoString(const std::string& path) {
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		std::cerr << "Could not open the file - '"
			<< path << "'" << std::endl;
		exit(EXIT_FAILURE);
	}
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int main()
{
	Parser parser = Parser();
	BlockNode blockNode = BlockNode();
	//std::string input = readFileIntoString("./test.cobr");
	//std::string input = "int i = 1; while (i != 4) { print(i); i = i + 1;}";
	//Lexer lexer = Lexer(input, "<stdin>");
	//auto returnLex = lexer.lex();
	//if (returnLex.second.m_errorName != "NULL")
	//	std::cout << returnLex.second;
	//else {
	//	std::vector<Token> tokenStream = returnLex.first;
	//	parser.parse(tokenStream, &blockNode);
	//	blockNode.run();
	//}


	while (true) {
		std::cout << "> ";
		std::string input = "";
		getline(std::cin, input);
		if (input == "exit") {
			std::cout << "do you want to quit? (y/n)" << std::endl;
			std::cin >> input;
			if (input == "y")
				return 0;
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
