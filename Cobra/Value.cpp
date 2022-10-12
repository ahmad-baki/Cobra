#include "Value.h"
#include "RuntimeError.h"
#include <format>
#include "Interpreter.h"
#include <sstream>

Value::Value()
	: data{nullptr}, typeId{0}
{}

Value::Value(Value* other, Error& outError, int typeId)
{
	if (typeId != 0 && other->getTypeId() != typeId) {
		Interpreter* interpreter = Interpreter::getSingelton();
		Type* othType = interpreter->getType(typeId, outError);
		Type* thisType = interpreter->getType(typeId, outError);
		if (othType == nullptr || thisType == nullptr) {
			return;
		}

		RuntimeError targetError = RuntimeError(std::format("Couldnt cast {} to {}",
			othType->getTypeName(), thisType->getTypeName()));
		outError.copy(targetError);
	}
	else {
		this->data		= other->getData();
		this->typeId	= other->getTypeId();
	}
}

Value::~Value()
{
	if (data != nullptr) {
		delete data;
	}
	data = nullptr;
}

int Value::getTypeId()
{
	return typeId;
}

void* Value::getData()
{
	return data;
}
