#include "GetVar.h"
#include <string>
#include "RuntimeError.h"
#include "Interpreter.h"

GetVar::GetVar(std::string varName, size_t line, size_t startColumn, size_t endColumn)
	: varName{varName}
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


Value* GetVar::run(Error& outError) {
	Value* val = Interpreter::getSingelton()->getVar(varName, outError);
	if (val == nullptr)
	{
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return nullptr;
	}
	return val;
}