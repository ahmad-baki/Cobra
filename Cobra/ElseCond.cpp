#include "ElseCond.h"
#include <format>
#include "RuntimeError.h"

ElseCond::ElseCond(Expression* cond, Statement* statement, size_t line, size_t startColumn, size_t endColumn)
	: cond{cond}, statement{ statement } 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}

bool ElseCond::run(Error& outError)
{
	std::vector<Value*> condVal = cond->run(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return false;
	}
	if (condVal.size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to boolean from list with size {}",
			std::to_string(condVal.size())) };
		outError.copy(targetError);
		return false;
	}

	bool condBol = condVal[0]->getBool(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return false;
	}
	if (condVal.size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to boolean from list with size {}",
			std::to_string(condVal.size())) };
		outError.copy(targetError);
		return false;
	}

	if (condBol) {
		statement->run(outError);

		if (outError.errType != ErrorType::NULLERROR)
			return false;

		return true;
	}
	return false;
}
