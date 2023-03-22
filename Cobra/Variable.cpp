#include "Variable.h"
#include "RuntimeError.h"
#include "Interpreter.h"
#include <format>

Variable::Variable(std::vector<Value*> values, Error& outError, int typeId,
	bool isConst, bool isStaticType, bool isList)
	: values{}, isConst{ isConst }, isStaticType{isStaticType}, isList{isList}
{
	//if (values.size() == 0) {
	//	RuntimeError targetError{ "empty list cannot be element of \
	//			list" };
	//}

	int copyTypeId = (isStaticType) ? typeId : 0;

	int size;
	if (copyTypeId != 0 && !isList) {
		size = 1;
	}
	else {
		size = values.size();
	}

	this->values.reserve(size);
	for (auto pointer = values.begin(); pointer < values.begin() + size; pointer++)
	{
		Value* copy = (*pointer)->getCopy(copyTypeId, outError);
		if (copy == nullptr) {
			return;
		}
		this->values.push_back(copy);
	}
}

void Variable::exit()
{
	for (Value* value : values) {
		if (value != nullptr) {
			delete value;
		}
	}
}

// when primVal == nullptr, it replaces the whole list
bool Variable::setVar(std::vector<Value*> tValues, Value* index, Error& outError)
{
	if (isConst) {
		RuntimeError targetError{ "Tried to write in const Variable" };
		outError.copy(targetError);
	}

	int typeId = (isStaticType) ? values[0]->getTypeId() : 0;
	if (index == nullptr) {

		if (!isList && tValues.size() != 1) {
			RuntimeError targetError{ std::format("target value has size {}, which is uneqal to 1"
				, tValues.size()) };
			outError.copy(targetError);
			return false;
		}

		//for (Value* valP : values) {
		//	delete valP;
		//}

		values.clear();
		values.reserve(tValues.size());
		for (Value* val : tValues)
		{
			Value* copy = val->getCopy(typeId, outError);
			if (copy == nullptr){
				return false;
			}
			values.push_back(copy);
		}
	}
	else {
		if (tValues.size() != 1) {
			RuntimeError targetError{ "a list cannot be element of another list" };
			outError.copy(targetError);
			return false;
		}

		int intIndex = getIndex(index, outError);
		if (outError.errorName != "NULL") {
			return false;
		}

		if (intIndex > values.size() - 1) {
			RuntimeError targetError{ std::format("IndexOutOfBounds: Cannot acces index {} of variable with size {}", intIndex, values.size()) };
			outError.copy(targetError);
			return false;
		}

		Value* copy = tValues[0]->getCopy(typeId, outError);
		if (copy == nullptr) {
			return false;
		}
		delete values[intIndex];
		values[intIndex] = copy;
	}
	return true;
}

// when primval == nullptr, it returns the whole list
std::vector<Value*> Variable::getVal(Value* index, Error& outError)
{
	// when value is not defined
	if (!isList && values.size() == 0) {
		RuntimeError targetError{ "ValueNotDefined: Cannot get undefined variable" };
		outError.copy(targetError);
		return {};
	}

	if (index == nullptr) {
		return values;
	}

	int intIndex = getIndex(index, outError);
	if (outError.errorName != "NULL") {
		return {};
	}

	if (intIndex > values.size()-1) {
		RuntimeError targetError{ std::format("IndexOutOfBounds: Cannot acces index {} \
			of variable with size {}", intIndex, values.size() )};
		outError.copy(targetError);
	}

	return { values[abs((int)(intIndex % values.size()))] };
}

// inefficient, will optimize it another time
int Variable::getIndex(Value* val, Error& outError)
{
	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	void* indexPointer = PrimValue::Cast(val->getData(), val->getTypeId(), intTypeId, outError);
	if (indexPointer == nullptr) {
		return 0;
	}
	int index = *(int*)indexPointer;
	delete indexPointer;
	return index;
}
