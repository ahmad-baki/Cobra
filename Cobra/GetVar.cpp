#include "GetVar.h"
#include <string>

template<SuppType T>
GetVar<T>::GetVar(std::string varName, SymbTable* table) 
	: varName{varName}, table{table} {}


template<SuppType T>
T GetVar<T>::run() {
	return table->run<T>(varName);
}

template class GetVar<bool>;
template class GetVar<int>;
template class GetVar<float>;