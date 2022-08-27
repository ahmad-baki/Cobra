#include "ElseCond.h"

ElseCond::ElseCond(Expression<bool>* cond, Statement* statement)
	: cond{cond}, statement{ statement } {}

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
