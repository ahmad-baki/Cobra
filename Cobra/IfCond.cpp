#include "IfCond.h"


IfCond::IfCond(Expression<bool>* cond, Statement* statement, std::vector<ElseCond*> ifElseStates, Statement* elseState)
	: cond{ cond }, statement{ statement }, ifElseStates{ ifElseStates }, elseState{ elseState }
{
}

void IfCond::run()
{
	if (cond->run()) {
		statement->run();
	}
	else {
		for (int i = 0; i < ifElseStates.size(); i++)
			if (ifElseStates[i]->run())
				return;
		if(elseState != nullptr)
			elseState->run();
	}
}

