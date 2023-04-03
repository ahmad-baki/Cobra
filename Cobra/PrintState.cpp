#include "PrintState.h"
#include <iostream>
#include "Value.h"

PrintState::PrintState(Expression* param)
	: param{param}
{
}

size_t PrintState::run(Error& outError) {
	std::vector<Value*> vals = param->run(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}
	std::cout << '[';
	for (size_t i = 0; i < vals.size(); i++) {
		std::cout << vals[i]->toString();
		if (i != vals.size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << ']' << std::endl;
	return 0;
}
//
//template class PrintState<bool>;
//template class PrintState<int>;
//template class PrintState<float>;