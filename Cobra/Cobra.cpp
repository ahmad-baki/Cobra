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
#include "Interpreter.h"

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
	std::cout << "executing: " << path << std::endl;
	std::string input = readFileIntoString(path);

	Lexer lexer = Lexer(input, path);
	Error error{};
	std::vector<Token> tokenString = lexer.lex(error);

	if (error.errorName != "NULL"){
		std::cout << error << '\n' << std::endl;
		return 0;
	}

	// neccesary to be before the parsing, so that std-types get loaded
	Interpreter* interpreter = Interpreter::getSingelton();

	Parser parser = Parser(input, path);
	std::vector<Statement*> statements = parser.parse(tokenString, error);

	if (error.errorName != "NULL") {
		std::cout << error << '\n' << std::endl;
		return 0;
	}
	interpreter->setStatements(statements);

	const auto startTime = system_clock::now();
	interpreter->run(error);
	const auto endTime = system_clock::now();
	const double runTime = duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (error.errorName != "NULL") {
		error.path = path;
		error.text = input;
		std::cout << error << '\n' << std::endl;
	}
	return runTime;
}

void execFromCommandLine() {
	size_t line = 1;
	Error error{};
	Interpreter* interpreter = Interpreter::getSingelton();
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
		std::vector<Token> tokenString = lexer.lex(error);
		if (error.errorName != "NULL"){
			std::cout << error << '\n' << std::endl;
			return;
		}

		Parser parser = Parser(input, "<stdin>");
		std::vector<Statement*> statements = parser.parse(tokenString, error);

		if (error.errorName != "NULL") {
			std::cout << error << '\n' << std::endl;
			return;
		}
		interpreter->addStatements(statements);
		interpreter->contin(error);
		if (error.errorName != "NULL") {
			error.path = "<stdin>";
			error.text = input;
			std::cout << error << '\n' << std::endl;
			return;
		}
		line++;
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1) {
		for (size_t i = 1; i < argc; i++) {
			double runTime = execFromFile(argv[i]);
			std::cout << "finished in: " << runTime << "ms" << std::endl;
		}
	}
	else {
		execFromCommandLine();
		std::cout << "finished" << std::endl;
	}	
	std::system("pause");
}
