#include "PrintState.h"
#include <iostream>

PrintState::PrintState(Expression* param)
	: param{param}
{
}

void PrintState::run(Error& outError) {
	Value* val = param->run(outError);

	if (val == nullptr)
		return;

	std::cout << *val << std::endl;
	return;
}
//
//template class PrintState<bool>;
//template class PrintState<int>;
//template class PrintState<float>;