#include "Loop.h"

Loop::Loop(Expression* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

void Loop::run(Error& outError)
{
	Value* condVal = cond->run(outError);

	if (condVal == nullptr)
		return;

	bool condBool = condVal->getBool(outError);

	if (outError.errorName != "NULL")
		return;

	while (condBool) {
		statement->run(outError);

		if (outError.errorName != "NULL")
			return;

		condVal = cond->run(outError);

		if (outError.errorName != "NULL")
			return;

		condBool = condVal->getBool(outError);

		if (outError.errorName != "NULL")
			return;
	}
}
