#include "IfCond.h"


IfCond::IfCond(Expression<bool>* cond, Statement* statement, std::vector<ElseCond*> ifElseStates, Statement* elseState)
	: cond{ cond }, statement{ statement }, ifElseStates{ ifElseStates }, elseState{ elseState }
{
}

Error IfCond::run()
{
	auto [condVal, condError] = cond->run();

	if (condError.m_errorName != "NULL")
		return { condError };

	if (condVal) {
		return statement->run();
	}
	else {
		for (int i = 0; i < ifElseStates.size(); i++)
		{
			auto [ifElseVal, ifElseError] = ifElseStates[i]->run();

			if (ifElseError.m_errorName != "NULL")
				return { ifElseError };

			if (ifElseVal) {
				return Error();
			}
		}
		if (elseState != nullptr) {
			return elseState->run();
		}
		return Error();
	}
}

