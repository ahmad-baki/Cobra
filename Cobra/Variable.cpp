#include "Variable.h"
#include "RuntimeError.h"
#include "Interpreter.h"
#include <format>

Variable::Variable(std::vector<Value*> values, Error& outError, int typeId,
	bool isConst, bool isStaticType, int size)
	: values{}, isConst{ isConst }, isStaticType{isStaticType}, size{size}
{
	//if (values.size() == 0) {
	//	RuntimeError targetError{ "empty list cannot be element of \
	//			list" };
	//}

	int copyTypeId = (isStaticType) ? typeId : 0;
	this->values.reserve(values.size());
	for (Value* val : values)
	{
		Value* copy = val->getCopy(copyTypeId, outError);
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

		if (size == -1 && tValues.size() != 1) {
			RuntimeError targetError{ std::format("value of size {} doesnt fit into variable of size 1"
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
			RuntimeError targetError{ std::format("list of size {} cannot be element of \
				list", tValues.size()) };
			outError.copy(targetError);
			return false;
		}

		int intIndex = getIndex(index, outError);
		if (outError.errorName != "NULL") {
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
	if (size == 1 && values.size() == 0) {
		RuntimeError targetError{ "ValueNotDefined: Cannot get not defined variable" };
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

	if (intIndex < 0 || intIndex > values.size()-1) {
		RuntimeError targetError{ std::format("IndexOutOfBounds: Cannot acces index {} \
			of variable with size {}", intIndex, values.size() )};
		outError.copy(targetError);
	}

	return { values[intIndex] };
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
