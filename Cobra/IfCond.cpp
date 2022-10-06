#include "IfCond.h"


IfCond::IfCond(Expression* cond, Statement* statement, std::vector<ElseCond*> ifElseStates, Statement* elseState)
	: cond{ cond }, statement{ statement }, ifElseStates{ ifElseStates }, elseState{ elseState }
{
}

void IfCond::run(Error& outError)
{
	Value* condVal = cond->run(outError);

	if (condVal == nullptr) {
		return;
	}

	bool boolean = condVal->getBool(outError);

	if (outError.errorName != "NULL")
		return;

	if (boolean) {
		statement->run(outError);
		return;
	}
	else {
		for (int i = 0; i < ifElseStates.size(); i++)
		{
			bool ifElseBool = ifElseStates[i]->run(outError);

			if (outError.errorName != "NULL")
				return;

			if (ifElseBool) {
				return;
			}
		}
		if (elseState != nullptr) {
			elseState->run(outError);
		}
		return;
	}
}

