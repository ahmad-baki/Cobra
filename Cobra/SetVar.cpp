#include "SetVar.h"
#include "RuntimeError.h"



SetVar::SetVar(std::string name, Expression* value, SymbTable* table, size_t line, size_t startColumn, size_t endColumn)
	: name{ name }, value{ value }
{
	this->table = table;
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


Error SetVar::run()
{
	auto [val, getError] = value->run();

	if (getError.m_errorName != "NULL")
		return getError;

	Error setError = table->set(name, val);
	if (setError.m_errorName != "NULL")
		return RuntimeError(setError.desc, line, startColumn, endColumn);
	return Error();
}