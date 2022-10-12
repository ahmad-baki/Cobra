#include "GetVal.h"
#include <string>
#include "RuntimeError.h"
#include "Interpreter.h"

GetVal::GetVal(std::string varName, size_t line, size_t startColumn, size_t endColumn)
	: varName{varName}
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


Value* GetVal::run(Error& outError) {
	Value* val = Interpreter::getSingelton()->getVar(varName, outError)->getVal();
	if (val == nullptr)
	{
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return nullptr;
	}
	return val;
}