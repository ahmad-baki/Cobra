#include "IfCond.h"
#include <format>
#include "RuntimeError.h"


IfCond::IfCond(Expression* cond, Statement* statement, std::vector<ElseCond*> ifElseStates, Statement* elseState)
	: cond{ cond }, statement{ statement }, ifElseStates{ ifElseStates }, elseState{ elseState }
{
}

size_t IfCond::run(Error& outError)
{
	std::vector<Value*> condVal = cond->run(outError);

	if (outError.errorName != "NULL") {
		return 0;
	}
	if (condVal.size() != 1) {
		RuntimeError targetError{ std::format("Cannot convert to boolean from list with size {}", 
			std::to_string(condVal.size())) };
	}

	bool condBol = condVal[0]->getBool(outError);

	if (outError.errorName != "NULL")
		return 0;

	if (condBol) {
		return statement->run(outError);
	}
	else {
		for (int i = 0; i < ifElseStates.size(); i++)
		{
			bool ifElseBool = ifElseStates[i]->run(outError);

			if (outError.errorName != "NULL") {
				return 0;
			}
			if (condVal.size() != 1) {
				RuntimeError targetError{ std::format("Cannot convert to boolean from list with size {}",
					std::to_string(condVal.size())) };
				outError.copy(targetError);
				return 0;
			}

			if (ifElseBool) {
				return ifElseStates[i]->run(outError);
			}
		}
		if (elseState != nullptr) {
			return elseState->run(outError);
		}
		return 0;
	}
}

