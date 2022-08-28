#include "SetVar.h"


template<SuppType T>
Error SetVar<T>::run()
{
	auto [val, error] = value->run();

	if (error.m_errorName != "NULL")
		return error;

	return table->set(name, val);
}

template<SuppType T>
SetVar<T>::SetVar(std::string name, Expression<T>* value, SymbTable* table)
	: name{name}, value{value} 
{
	this->table = table;
}


template class SetVar<int>;
template class SetVar<float>;