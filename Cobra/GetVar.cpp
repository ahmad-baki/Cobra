#include "GetVar.h"
#include <string>
#include "RuntimeError.h"

template<SuppType T>
GetVar<T>::GetVar(std::string varName, SymbTable* table, size_t line, size_t startColumn, size_t endColumn)
	: varName{varName}, table{table} 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


template<SuppType T>
std::pair<T, Error> GetVar<T>::run() {
	auto [val, error] = table->run<T>(varName);
	if (error.m_errorName != "NULL")
		return { {}, RuntimeError(error.desc, this->line, this->startColumn, this->endColumn) };
	return { val, Error() };
}

template class GetVar<bool>;
template class GetVar<int>;
template class GetVar<float>;