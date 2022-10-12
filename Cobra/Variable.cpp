#include "Variable.h"
#include "RuntimeError.h"

Variable::Variable(Value* tVal, Error& outError, int typeId, 
	bool isConst, bool isStaticType)
	: value{nullptr}, isConst{isConst}, isStaticType{isStaticType}
{
	if (tVal != nullptr) {
		value = tVal->getCopy(typeId, outError);
	}
}

void Variable::exit()
{
	if (value != nullptr) {
		delete value;
	}
	value = nullptr;
}

bool Variable::setVar(Value* tVal, Error& outError)
{
	if (isConst) {
		RuntimeError targetError{ "Tried to write in const Variable",
			tVal->line, tVal->startColumn, tVal->endColumn };
		outError.copy(targetError);
	}

	int tTypeId{ 0 };
	if (isStaticType) {
		tTypeId = value->getTypeId();
	}

	if (value != nullptr) {
		delete value;
	}

	value = tVal->getCopy(tTypeId, outError);

	if (value == nullptr) {
		return false;
	}
	return true;
}

Value* Variable::getVal()
{
	return value;
}
