#include "SetVar.h"


template<SuppType T>
void SetVar<T>::run()
{
	table->set(name, value->run());
}

template<SuppType T>
SetVar<T>::SetVar(std::string name, Expression<T>* value, SymbTable* table)
	: name{name}, value{value} 
{
	this->table = table;
}


template class SetVar<int>;
template class SetVar<float>;