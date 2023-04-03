#include "GetVal.h"
#include <string>
#include "RuntimeError.h"
#include "Interpreter.h"

GetVar::GetVar(std::string varName, Expression* expr, size_t line, size_t startColumn, size_t endColumn)
	: varName{varName}, expr{ expr }
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}


std::vector<Value*> GetVar::run(Error& outError) {
	Value* index{nullptr};
	if (expr != nullptr) {
		std::vector<Value*> vals = expr->run(outError);
		if (vals.size() == 0) {
			return {};
		}
		if (vals.size() > 1) {
			Error targetError{ ErrorType::RUNTIMEERROR, "InvalidType: Cannot use list as index" };
			outError.copy(targetError);
			return {};
		}
		index = vals[0];
	}
	Variable* var = Interpreter::getSingelton()->getVar(varName, outError);
	if (var == nullptr)
	{
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return {};
	}
	return var->getVal(index, outError);
}