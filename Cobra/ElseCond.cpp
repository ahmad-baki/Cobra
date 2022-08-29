#include "ElseCond.h"

ElseCond::ElseCond(Expression<bool>* cond, Statement* statement, size_t line, size_t startColumn, size_t endColumn)
	: cond{cond}, statement{ statement } 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}

std::pair<bool, Error> ElseCond::run()
{
	auto [val, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return { {}, condError };

	if (val) {
		Error stateError = statement->run();

		if (stateError.m_errorName != "NULL")
			return { {}, stateError };

		return { true, Error() };
	}
	return { false, Error() };
}
