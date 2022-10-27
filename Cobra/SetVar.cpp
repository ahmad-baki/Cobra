#include "SetVar.h"
#include "RuntimeError.h"
#include "Interpreter.h"
#include <format>



SetVar::SetVar(std::string name, Expression* value, size_t line, size_t startColumn, size_t endColumn)
	: name{ name }, value{ value }
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn = endColumn;
}


void SetVar::run(Error& outError)
{
	std::vector<Value*> val = value->run(outError);

	if (outError.errorName != "NULL") {
		return;
	}
	if (val.size() != 1) {
		RuntimeError targetError{ std::format("Cannot convert to boolean from list with size {}",
			std::to_string(val.size())) };
		outError.copy(targetError);
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