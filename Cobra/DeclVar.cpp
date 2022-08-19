#include "DeclVar.h"


template<SuppType T>
DeclVar<T>::DeclVar(std::string name, SymbTable* table)
	: name{name}
{
	throw std::exception("invalid type");
}

template<>
DeclVar<bool>::DeclVar(std::string name, SymbTable* table)
	: name{ name }
{
	Init(table);

	// for expr purp
	table->reg(name, 0);
}

template<>
DeclVar<int>::DeclVar(std::string name, SymbTable* table)
	: name{ name }
{
	Init(table);

	// for expr purp
	table->reg(name, 0);
}

template<>
DeclVar<float>::DeclVar(std::string name, SymbTable* table)
	: name{ name }
{
	Init(table);

	// for expr purp
	table->reg(name, 1);
}

template<SuppType T>
void DeclVar<T>::Init(SymbTable* table) 
{
	this->table = table;
}

template<SuppType T>
inline void DeclVar<T>::run()
{
	table->declare<T>(name, 0);
}


template class DeclVar<bool>;
template class DeclVar<int>;
template class DeclVar<float>;