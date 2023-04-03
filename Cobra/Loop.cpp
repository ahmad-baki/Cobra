#include "Loop.h"
#include <format>
#include "RuntimeError.h"

Loop::Loop(Expression* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

size_t Loop::run(Error& outError)
{	std::vector<Value*> condVal = cond->run(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return 0;
	}
	if (condVal.size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to boolean from list with size {}", 
			std::to_string(condVal.size())) };
		outError.copy(targetError);
		return 0;
	}

	bool condBool = condVal[0]->getBool(outError);

	if (outError.errType != ErrorType::NULLERROR)
		return 0;

	while (condBool) {
		size_t breakCount = statement->run(outError);

		if (outError.errType != ErrorType::NULLERROR)
			return 0;

		if (breakCount > 0) {
			return breakCount - 1;
		}

		condVal = cond->run(outError);

		if (outError.errType != ErrorType::NULLERROR) {
			return 0;
		}

		if (condVal.size() != 1) {
			Error targetError{ ErrorType::RUNTIMEERROR, std::format("Cannot convert to boolean from list with size {}",
				std::to_string(condVal.size())) };
			outError.copy(targetError);
			return 0;
		}

		condBool = condVal[0]->getBool(outError);

		if (outError.errType != ErrorType::NULLERROR)
			return 0;
	}
}
