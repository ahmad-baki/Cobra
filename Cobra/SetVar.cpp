#include "SetVar.h"
#include "RuntimeError.h"
#include "Interpreter.h"
#include <format>



SetVar::SetVar(std::string name, Expression* index, Expression* tValue, size_t line, size_t startColumn, size_t endColumn)
	: name{ name }, index{ index }, tValue{tValue}
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn = endColumn;
}


size_t SetVar::run(Error& outError)
{
	std::vector<Value*> val = tValue->run(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}
	// confusion 1
	//if (val.size() != 1) {
	//	Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to boolean from list with size {}",
	//		std::to_string(val.size())) };
	//	outError.copy(targetError);
	//	return;
	//}

	Value* indexVal;
	if (index != nullptr) {
		std::vector<Value*> indexVals = index->run(outError);
		if (outError.errType != ErrorType::NULLERROR) {
			return 0;
		}
		if (indexVals.size() != 1) {
			Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to index from list with size {}",
				std::to_string(val.size())) };
			outError.copy(targetError);
			return 0;
		}
		indexVal = indexVals[0];
	}
	else {
		indexVal = nullptr;
	}

	Interpreter::getSingelton()->setVar(name, indexVal, val, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
	}
	return 0;
}

size_t SetVar::run(std::vector<Value*> val, Error& outError) {

	Interpreter::getSingelton()->setVar(name, nullptr, val, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		outError.line = line;
		outError.startColumn = startColumn;
		outError.endColumn = endColumn;
	}
	return 0;
}