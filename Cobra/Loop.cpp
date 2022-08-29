#include "Loop.h"

Loop::Loop(Expression<bool>* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

Error Loop::run()
{
	auto [val, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return { condError };

	while (val) {
		Error stateError = statement->run();

		if (stateError.m_errorName != "NULL")
			return stateError;

		auto condReturn = cond->run();
		val = condReturn.first;
		condError = condReturn.second;

		if (condError.m_errorName != "NULL")
			return { condError };
	}
	return Error();
}
