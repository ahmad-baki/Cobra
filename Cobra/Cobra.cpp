#include <iostream>
#include <windows.h>
#include <string>
#include "Parser.h"
#include "Lexer.h"

int main()
{
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
			blockNode.run();
		}
	}
}
