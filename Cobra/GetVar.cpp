#include "GetVar.h"
#include <string>
#include "RuntimeError.h"

GetVar::GetVar(std::string varName, SymbTable* table, size_t line, size_t startColumn, size_t endColumn)
	: varName{varName}, table{table} 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


Value* GetVar::run(Error& outError) {
	Value* val = table->run(varName, outError);
	if (val == nullptr)
	{
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return nullptr;
	}
	return val;
}