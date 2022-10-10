#include "Value.h"
#include "RuntimeError.h"
#include <format>
#include "TokenType.cpp"
#include <vector>
#include <string>
#include "Interpreter.h"



Value::Value()
	: typeId{ 0 }, data{ nullptr }, isConst{ false },
	isStaticType{ false }
{
}

Value::Value(int typeId, void* data, Error& outError, bool isConst, bool isStaticType)
	: typeId{ typeId }, data{ data }, isConst{ isConst },
	isStaticType{ isStaticType }
{
	this->typeId = typeId;
	//this->data = castVal;
	this->data = data;
}

Value::Value(Value* other, Error& outError, int typeId, bool isConst, bool isStaticType)
	: isConst{ isConst }, isStaticType{ isStaticType }
{
	if (other == nullptr) {
		this->typeId	= typeId;
		this->data		= nullptr;
		return;
	}
	if (typeId == 0 || typeId == other->getType()) {
		this->typeId	= other->getType();
		this->data		= other->data;
		return;
	}
	void* castVal = Cast(other->data, typeId, this->typeId, outError);

	if (castVal == nullptr) {
		return;
	}

	this->typeId	= typeId;
	this->data		= castVal;
}

Value::~Value() {
	if (data != nullptr) {
		delete data;
	}
}

Value* Value::run(Error& outError) {
	return this;
}

void Value::setVal(Value* val, Error& outError) {
	if (isConst) {
		RuntimeError targetError{ "Tried to write in const Variable", 
			line, startColumn, endColumn };
		outError.copy(targetError);
	}
	void* castVal = Cast(val->getData(), val->getType(), this->typeId, outError);
	if (castVal == nullptr)
		return;

	delete this->data;
	this->data = castVal;

	//else if (this->typeId == 0) {
	//	if(data != nullptr)
	//		delete this->data;
	//	this->data = val->getData();
	//	this->typeId = val->getType();
	//}
	//else if (isStaticType && typeId != val->getType()) {
	//	void* castVal = Cast(val->getData(), val->getType(), this->typeId, outError);
	//	if (castVal == nullptr)
	//		return;

	//	delete this->data;
	//	this->data = castVal;
	//}
	//else {
	//	if (data != nullptr)
	//		delete this->data;
	//	this->data = val->getData();
	//	this->typeId = val->getType();
	//}
}

bool Value::getBool(Error& outError)
{
	Error targetError{};
	Interpreter* interpr = Interpreter::getSingelton();
	if (typeId == interpr->getTypeId("int", outError)) {
		return bool(*(int*)data);
	}
	if (typeId == interpr->getTypeId("int", outError)) {
		return bool(*(float*)data);
	}
	if (typeId == 0) {
		targetError = RuntimeError("Cannot acces data of variable which is not defined",
			line, startColumn, endColumn);
		outError.copy(targetError);
		return false;
	}
	targetError = RuntimeError("Invalid DataType", line, startColumn, endColumn);
	outError.copy(targetError);
	return false;
}

int Value::getType()
{
	return typeId;
}

template<SuppType T>
Value* Value::calcOp(T* val1, T* val2, enum TokenType op, int typeId, Error& outError) {
	switch (op)
	{
	case TokenType::PLUS:
		return new Value(typeId, new T( *val1 + *val2 ), outError);
	case TokenType::MINUS:
		return new Value(typeId, new T( *val1 - *val2 ), outError);
	case TokenType::DIV:
		if (*val2 == 0) {
			RuntimeError targetError = RuntimeError("ZeroDivisionError: " + 
				std::to_string(*val1) + "/0 ", line, startColumn, endColumn);
			outError.copy(targetError);
			return nullptr;
		}
		return new Value(typeId, new T( *val1 / *val2 ), outError);
	case TokenType::MUL:
		return new Value(typeId, new T( *val1 * *val2 ), outError);
	case TokenType::EQEQ:
		return new Value(typeId, new T( *val1 == *val2 ), outError);
	case TokenType::EXCLAEQ:
		return new Value(typeId, new T( *val1 != *val2 ), outError);
	case TokenType::SMALL:
		return new Value(typeId, new T( *val1 < *val2 ), outError);
	case TokenType::SMALLEQ:
		return new Value(typeId, new T( *val1 <= *val2 ), outError);
	case TokenType::BIG:
		return new Value(typeId, new T( *val1 > *val2 ), outError);
	case TokenType::BIGEQ:
		return new Value(typeId, new T( *val1 >= *val2 ), outError);
	case TokenType::AND:
		return new Value(typeId, new T( *val1 && *val2 ), outError);
	case TokenType::OR:
		return new Value(typeId, new T( *val1 || *val2 ), outError);
	case TokenType::MOD:
		if (val2 == 0) {
			RuntimeError targetError = RuntimeError("ZeroModError: " + 
				std::to_string(*val1) + "/0", line, startColumn, endColumn);
			return nullptr;
		}
		return new Value(typeId, new T(std::fmod(*val1, *val2)), outError);
	}

	RuntimeError targetError = RuntimeError("Invalid Operator: " + std::to_string(op),
		line, startColumn, endColumn);
	outError.copy(targetError);
	return nullptr;
}

////template<>
//Value* Value::calcOp(float* val1, float* val2, enum TokenType op, Error& outError) {
//	switch (op)
//	{
//	case TokenType::PLUS:
//		return new Value(Value::DECTYPE, new float{*val1 + *val2}, outError);
//	case TokenType::MINUS:
//		return new Value(Value::DECTYPE, new float{ *val1 - *val2 }, outError);
//	case TokenType::DIV:
//		if (val2 == 0) {
//			RuntimeError targetError = RuntimeError("ZeroDivisionError: " + std::to_string(*val1) + "/0",
//				line, startColumn, endColumn);
//			outError.copy(targetError);
//			return nullptr;
//		}
//		return new Value(Value::DECTYPE, new float{ *val1 / *val2 }, outError);
//	case TokenType::MUL:
//		return new Value(Value::DECTYPE, new float{ *val1 * *val2 }, outError);
//	case TokenType::EQEQ:
//		return new Value(Value::DECTYPE, new float(*val1 == *val2), outError);
//	case TokenType::EXCLAEQ:
//		return new Value(Value::DECTYPE, new float(*val1 != *val2), outError);
//	case TokenType::SMALL:
//		return new Value(Value::DECTYPE, new float(*val1 < *val2), outError);
//	case TokenType::SMALLEQ:
//		return new Value(Value::DECTYPE, new float(*val1 <= *val2), outError);
//	case TokenType::BIG:
//		return new Value(Value::DECTYPE, new float(*val1 > *val2), outError);
//	case TokenType::BIGEQ:
//		return new Value(Value::DECTYPE, new float(*val1 >= *val2), outError);
//	case TokenType::AND:
//		return new Value(Value::DECTYPE, new float(*val1 && *val2), outError);
//	case TokenType::OR:
//		return new Value(Value::DECTYPE, new float(*val1 || *val2), outError);
//	case TokenType::MOD:
//		if (val2 == 0) {
//			RuntimeError targetError = RuntimeError("ZeroModError: " + std::to_string(*val1) +"/{}",
//				line, startColumn, endColumn);
//			outError.copy(targetError);
//			return nullptr;
//		}
//		return new Value(Value::DECTYPE, new float{ std::fmod(*val1, *val2) }, outError);
//	}
//
//	RuntimeError targetError = RuntimeError("Invalid Operator: ", line, startColumn, endColumn);
//	outError.copy(targetError);
//	return nullptr;
//}

Value* Value::doOp(Value& other, enum TokenType op, Error& outError)
{
	if (data == nullptr) {
		RuntimeError targetError = RuntimeError("Cannot acces data of variable which is not defined", 
			line, startColumn, endColumn);
		outError.copy(targetError);
		return nullptr;
	}

	void* castVal = Cast(other.data, other.typeId, this->typeId, outError);
	if (castVal == nullptr)
		return nullptr;
	Value* result{ nullptr };

	Interpreter* interpreter = Interpreter::getSingelton();
	int intTypeId = interpreter->getTypeId("int", outError);
	int floatTypeId = interpreter->getTypeId("float", outError);
	if (this->typeId == intTypeId) {
		result = calcOp((int*)data, (int*)castVal, op, intTypeId, outError);
	}
	else if (this->typeId == floatTypeId) {
		result = calcOp((float*)data, (float*)castVal, op, floatTypeId, outError);
	}
	else{
		RuntimeError targetError = RuntimeError("Invalid DataType", line, startColumn, endColumn);
		outError.copy(targetError);
	}
	delete castVal;
	return result;
}

void* Value::getData() {
	return data;
}


void* Value::Cast(void* data, int o_typeId, int t_typeId, Error& outError) {
	// checks viable conversions
	void* returnValue{ nullptr };
	Interpreter* interpreter = Interpreter::getSingelton();
	int intTypeId = interpreter->getTypeId("int", outError);
	int floatTypeId = interpreter->getTypeId("float", outError);

	if (t_typeId == intTypeId) {
		if (o_typeId == intTypeId){
			returnValue = (void*)new int{ *(int*)data };
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new int(*(float*)data);
		}
	}
	else if (t_typeId == floatTypeId) {
		if (o_typeId == intTypeId) {
			returnValue = (void*)new float(*(int*)data);
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new float{ *(float*)data };
		}
	}
	if (returnValue == nullptr) {
		RuntimeError targetError = RuntimeError("Couldnt cast " + 
			std::to_string(o_typeId) + " to {}" + std::to_string(t_typeId));
		return nullptr;
	}
	return returnValue;
}

std::ostream& operator<<(std::ostream& output, const Value& e)
{
	if (e.data == nullptr)
	{
		output << "NULL";
	}
	else {
		Error outError{};
		Interpreter* interpr = Interpreter::getSingelton();
		if		(e.typeId == interpr->getTypeId("int", outError)) {
			output << *(int*)e.data;
		}
		else if (e.typeId == interpr->getTypeId("float", outError)) {
			output << *(float*)e.data;
		}
		else if (e.typeId == 0) {
			output << "undefined";
		}
	}
	return output;
}
