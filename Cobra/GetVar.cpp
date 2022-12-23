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
			RuntimeError targetError{ "InvalidType: Cannot use list as index" };
			outError.copy(targetError);
			return {};
		}
		index = vals[0];
	}
	std::vector<Value*> val = Interpreter::getSingelton()->getVar(varName, outError)->getVal(index, outError);
	if (val.size() == 0)
	{
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return {};
	}
	return val;
}