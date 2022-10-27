#include "PrintState.h"
#include <iostream>
#include "Value.h"

PrintState::PrintState(Expression* param)
	: param{param}
{
}

void PrintState::run(Error& outError) {
	std::vector<Value*> vals = param->run(outError);

	if (outError.errorName != "NULL") {
		return;
	}
	std::cout << '[';
	for (Value* val : vals) {
		std::cout << val->toString();
	}
	std::cout << ']' << std::endl;
	return;
}
//
//template class PrintState<bool>;
//template class PrintState<int>;
//template class PrintState<float>;