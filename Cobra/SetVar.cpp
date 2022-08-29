#include "SetVar.h"
#include "RuntimeError.h"


template<SuppType T>
SetVar<T>::SetVar(std::string name, Expression<T>* value, SymbTable* table, size_t line, size_t startColumn, size_t endColumn)
	: name{ name }, value{ value }
{
	this->table = table;
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


template<SuppType T>
Error SetVar<T>::run()
{
	auto [val, getError] = value->run();

	if (getError.m_errorName != "NULL")
		return getError;

	Error setError = table->set(name, val);
	if (setError.m_errorName != "NULL")
		return RuntimeError(setError.desc, line, startColumn, endColumn);
	return Error();
}

template class SetVar<int>;
template class SetVar<float>;