#include "Loop.h"

Loop::Loop(Expression* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

Error Loop::run()
{
	auto [condVal, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return condError;

	auto [boolean, booleanError] = condVal->getBool();

	if (booleanError.m_errorName != "NULL")
		return booleanError;

	while (boolean) {
		Error stateError = statement->run();

		if (stateError.m_errorName != "NULL")
			return stateError;

		auto [condVal, condError] = cond->run();

		if (condError.m_errorName != "NULL")
			return condError;

		auto [boolean, booleanError] = condVal->getBool();

		if (booleanError.m_errorName != "NULL")
			return booleanError;
	}
	return Error();
}
