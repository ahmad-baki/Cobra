#include "PrintState.h"
#include <iostream>
#include "Value.h"

PrintState::PrintState(Expression* param)
	: param{param}
{
}

void PrintState::run(Error& outError) {
	Value* val = param->run(outError);

	if (val == nullptr)
		return;

	std::cout << val->toString() << std::endl;
	return;
}
//
//template class PrintState<bool>;
//template class PrintState<int>;
//template class PrintState<float>;