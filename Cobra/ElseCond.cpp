#include "ElseCond.h"

ElseCond::ElseCond(Expression* cond, Statement* statement, size_t line, size_t startColumn, size_t endColumn)
	: cond{cond}, statement{ statement } 
{
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
}

std::pair<bool, Error> ElseCond::run()
{
	auto [condVal, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return { {}, condError };

	auto [boolean, booleanError] = condVal->getBool();

	if (booleanError.m_errorName != "NULL")
		return { {}, booleanError };

	if (boolean) {
		Error stateError = statement->run();

		if (stateError.m_errorName != "NULL")
			return { {}, stateError };

		return { true, Error() };
	}
	return { false, Error() };
}
