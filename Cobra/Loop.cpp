#include "Loop.h"
#include <format>
#include "RuntimeError.h"

Loop::Loop(Expression* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

void Loop::run(Error& outError)
{	std::vector<Value*> condVal = cond->run(outError);

	if (outError.errorName != "NULL") {
		return;
	}
	if (condVal.size() != 1) {
		RuntimeError targetError{ std::format("Cannot convert to boolean from list with size {}", 
			std::to_string(condVal.size())) };
		outError.copy(targetError);
		return;
	}

	bool condBool = condVal[0]->getBool(outError);

	if (outError.errorName != "NULL")
		return;

	while (condBool) {
		statement->run(outError);

		if (outError.errorName != "NULL")
			return;

		condVal = cond->run(outError);

		if (outError.errorName != "NULL") {
			return;
		}

		if (condVal.size() != 1) {
			RuntimeError targetError{ std::format("Cannot convert to boolean from list with size {}",
				std::to_string(condVal.size())) };
			outError.copy(targetError);
			return;
		}

		condBool = condVal[0]->getBool(outError);

		if (outError.errorName != "NULL")
			return;
	}
}
