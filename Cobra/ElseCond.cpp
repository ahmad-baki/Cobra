#include "ElseCond.h"

ElseCond::ElseCond(Expression* cond, Statement* statement, size_t line, size_t startColumn, size_t endColumn)
	: cond{cond}, statement{ statement } 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}

bool ElseCond::run(Error& outError)
{
	Value* condVal = cond->run(outError);

	if (outError.errorName != "NULL") {
		//outError.line = line;
		//outError.startColumn = startColumn;
		//outError.endColumn = endColumn;
		return false;
	}

	bool condBol = condVal->getBool(outError);

	if (outError.errorName != "NULL") {
		//outError.line = line;
		//outError.startColumn = startColumn;
		//outError.endColumn = endColumn;
		return false;
	}

	if (condBol) {
		statement->run(outError);

		if (outError.errorName != "NULL")
			return false;

		return true;
	}
	return false;
}
