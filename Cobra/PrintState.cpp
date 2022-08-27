#include "PrintState.h"
#include <iostream>

template<SuppType T>
inline PrintState<T>::PrintState(Expression<T>* param)
	: param{param}
{
}

template<SuppType T>
Error PrintState<T>::run() {
	auto [val, error] = param->run();

	if (error.m_errorName != "NULL")
		return error;

	std::cout << val << std::endl;
	return Error();
}

template class PrintState<bool>;
template class PrintState<int>;
template class PrintState<float>;