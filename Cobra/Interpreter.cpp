#include "Interpreter.h"

Interpreter::Interpreter()
	: blockNode{nullptr}
{
}

Interpreter* Interpreter::getSingelton() {
	if (singelton == nullptr) {
		singelton = new Interpreter();
	}
	return singelton;
}

void Interpreter::run(Error& outError) {
	blockNode->run(outError);
}

void Interpreter::contin(Error& outError) {
	blockNode->contin(outError);
}

void Interpreter::addStatements(std::vector<Statement*> statements) {
	for (Statement* statement : statements) {
		blockNode->add(statement);
	}
}

void Interpreter::addStatement(Statement* statement) {
	blockNode->add(statement);
}

void Interpreter::setStatements(BlockNode* blockNode) {
	this->blockNode = blockNode;
}