#include "SetVar.h"
#include "RuntimeError.h"
#include "Interpreter.h"



SetVar::SetVar(std::string name, Expression* value, size_t line, size_t startColumn, size_t endColumn)
	: name{ name }, value{ value }
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn = endColumn;
}


void SetVar::run(Error& outError)
{
	Value* val = value->run(outError);

	if (val == nullptr) {
		//outError.line			= line;
		//outError.startColumn	= startColumn;
		//outError.endColumn		= endColumn;
		return;
	}

	Interpreter::getSingelton()->setVar(name, val, outError);
	if (outError.errorName != "NULL") {
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
	}
	return;
}