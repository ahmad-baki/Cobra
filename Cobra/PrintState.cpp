#include "PrintState.h"
#include <iostream>

template<SuppType T>
inline PrintState<T>::PrintState(Expression<T>* param)
	: param{param}
{
}

template<SuppType T>
void PrintState<T>::run() {
	std::cout << param->run() << std::endl;
}

template class PrintState<bool>;
template class PrintState<int>;
template class PrintState<float>;