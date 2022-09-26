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


std::pair<Value*, Error> GetVar::run() {
	auto [val, error] = table->run(varName);
	if (error.m_errorName != "NULL")
		return { {}, RuntimeError(error.desc, this->line, this->startColumn, this->endColumn) };
	return { val, Error() };
}