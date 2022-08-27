#include "Loop.h"

Loop::Loop(Expression<bool>* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

Error Loop::run()
{
	auto [val, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return { condError };

	while (val) {
		return statement->run();
	}
}
