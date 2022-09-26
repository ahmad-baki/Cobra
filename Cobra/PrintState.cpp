#include "PrintState.h"
#include <iostream>

PrintState::PrintState(Expression* param)
	: param{param}
{
}

Error PrintState::run() {
	auto [val, error] = param->run();

	if (error.m_errorName != "NULL")
		return error;

	std::cout << *val << std::endl;
	return Error();
}
//
//template class PrintState<bool>;
//template class PrintState<int>;
//template class PrintState<float>;