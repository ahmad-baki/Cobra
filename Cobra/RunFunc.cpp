#include "RunFunc.h"
#include "Interpreter.h"

RunFunc::RunFunc(std::string funcName, std::vector<Expression*> params) 
	: funcName{funcName}, params{params}
{
}

std::vector<Value*> RunFunc::run(Error& outError) {
	std::vector<std::vector<Value*>> paramVal{};
	paramVal.reserve(params.size());
	for (auto i = params.begin(); i != params.end(); i++) {
		paramVal.push_back((*i)->run(outError));

		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}

	return Interpreter::getSingelton()->callFunc(funcName, paramVal, outError);
}