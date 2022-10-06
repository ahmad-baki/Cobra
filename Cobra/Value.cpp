#include "Value.h"
#include "RuntimeError.h"
#include <format>
#include "TokenType.cpp"
#include <vector>

Value::Value()
	: dataType{ UNDEFINED }, data{ nullptr }, isConst{ false }, 
	isStaticType{ false }
{
}

Value::Value(enum DataType dataType, void* data, Error& outError, bool isConst, bool isStaticType)
	: dataType{ dataType }, data{ data }, isConst{ isConst }, 
	isStaticType{ isStaticType }
{
	if (data == nullptr || dataType == -1) {
		this->dataType = dataType;
		this->data = nullptr;
		return;
	}
	void* castVal = Cast(data, dataType, this->dataType, outError);

	if (castVal == nullptr) {
		return;
	}

	this->dataType = dataType;
	this->data = castVal;
}

Value::Value(Value* other, Error& outError, enum DataType dataType, bool isConst, bool isStaticType)
	: isConst{ isConst }, isStaticType{ isStaticType }
{
	if (other == nullptr) {
		this->dataType	= dataType;
		this->data		= nullptr;
		return;
	}
	if (dataType == -1 || dataType == other->getType()) {
		this->dataType	= other->getType();
		this->data		= other->data;
		return;
	}
	void* castVal = Cast(other->data, dataType, this->dataType, outError);

	if (castVal == nullptr) {
		return;
	}

	this->dataType	= dataType;
	this->data		= castVal;
}

Value::Value(Value& old_obj) {
	dataType		= old_obj.dataType;
	data			= old_obj.data;
	isConst			= old_obj.isConst;
	isStaticType	= old_obj.isStaticType;
}

Value::~Value() {
	//if (isIter) {
	//	std::vector<Value*> arr = *(std::vector<Value*>*)data;
	//	for (Value* valP : arr) {
	//		delete valP;
	//	}

	//}
	delete data;
}

Value* Value::run(Error& outError) {
	return this;
}

void Value::setVal(Value* val, Error& outError) {
	if (isConst) {
		RuntimeError targetError = RuntimeError("Tried to write in const Variable", line, startColumn, endColumn);
		outError.copy(targetError);
	}
	if (this->dataType == UNDEFINED) {
		if(data != nullptr)
			delete this->data;
		this->data = val->getData();
		this->dataType = val->getType();
	}
	else if (isStaticType && dataType != val->getType()) {
		void* castVal = Cast(val->getData(), val->getType(), this->dataType, outError);
		if (castVal == nullptr)
			return;

		delete this->data;
		this->data = castVal;
	}
	else {
		if (data != nullptr)
			delete this->data;
		this->data = val->getData();
		this->dataType = val->getType();
	}
}

bool Value::getBool(Error& outError)
{
	Error targetError{};
	switch (dataType)
	{
	case 0:
		return bool(*(int*)data);
	case 1:
		return bool(*(float*)data);
	case -1:
		targetError = RuntimeError("Cannot acces data of variable which is not defined", 
			line, startColumn, endColumn);
		outError.copy(targetError);
		return false;
	default:
		targetError = RuntimeError("Invalid DataType", line, startColumn, endColumn);
		outError.copy(targetError);
		return false;
	}
}

Value::DataType Value::getType()
{
	return dataType;
}

template<>
Value* Value::calcOp(int* val1, int* val2, enum TokenType op, Error& outError) {
	switch (op)
	{
	case TokenType::PLUS:
		new Value(Value::INTTYPE, new int{*val1 + *val2}, outError);
	case TokenType::MINUS:
		new Value(Value::INTTYPE, new int{ *val1 - *val2 }, outError);
	case TokenType::DIV:
		if (*val2 == 0) {
			RuntimeError targetError = RuntimeError(std::format("ZeroDivisionError: {}/{}",
				std::to_string(*val1), "0"), line, startColumn, endColumn);
			outError.copy(targetError);
			return nullptr;
		}
		new Value(Value::INTTYPE, new int{ *val1 / *val2 }, outError);
	case TokenType::MUL:
		new Value(Value::INTTYPE, new int{ *val1 * *val2 }, outError);
	case TokenType::EQEQ:
		new Value(Value::INTTYPE, new int{ *val1 == *val2 }, outError);
	case TokenType::EXCLAEQ:
		new Value(Value::INTTYPE, new int{ *val1 != *val2 }, outError);
	case TokenType::SMALL:
		new Value(Value::INTTYPE, new int{ *val1 < *val2 }, outError);
	case TokenType::SMALLEQ:
		new Value(Value::INTTYPE, new int{ *val1 <= *val2 }, outError);
	case TokenType::BIG:
		new Value(Value::INTTYPE, new int{ *val1 > *val2 }, outError);
	case TokenType::BIGEQ:
		new Value(Value::INTTYPE, new int{ *val1 >= *val2 }, outError);
	case TokenType::AND:
		new Value(Value::INTTYPE, new int{ *val1 && *val2 }, outError);
	case TokenType::OR:
		new Value(Value::INTTYPE, new int{ *val1 || *val2 }, outError);
	case TokenType::MOD:
		if (val2 == 0) {
			RuntimeError targetError = RuntimeError(std::format("ZeroModError: {}/{}",
				std::to_string(*val1), "0"), line, startColumn, endColumn);
			return nullptr;
		}
		return new Value(Value::INTTYPE, new int(std::fmod(*val1, *val2)), outError);
	}

	RuntimeError targetError = RuntimeError(std::format("Invalid Operator: {}",
		std::to_string(op)), line, startColumn, endColumn);
	outError.copy(targetError);
	return nullptr;
}

template<>
Value* Value::calcOp(float* val1, float* val2, enum TokenType op, Error& outError) {
	switch (op)
	{
	case TokenType::PLUS:
		return new Value(Value::DECTYPE, new float{*val1 + *val2}, outError);
	case TokenType::MINUS:
		return new Value(Value::DECTYPE, new float{ *val1 - *val2 }, outError);
	case TokenType::DIV:
		if (val2 == 0) {
			RuntimeError targetError = RuntimeError(std::format("ZeroDivisionError: {}/{}", std::to_string(*val1), '0'), 
				line, startColumn, endColumn);
			outError.copy(targetError);
			return nullptr;
		}
		return new Value(Value::DECTYPE, new float{ *val1 / *val2 }, outError);
	case TokenType::MUL:
		return new Value(Value::DECTYPE, new float{ *val1 * *val2 }, outError);
	case TokenType::EQEQ:
		return new Value(Value::DECTYPE, new float(*val1 == *val2), outError);
	case TokenType::EXCLAEQ:
		return new Value(Value::DECTYPE, new float(*val1 != *val2), outError);
	case TokenType::SMALL:
		return new Value(Value::DECTYPE, new float(*val1 < *val2), outError);
	case TokenType::SMALLEQ:
		return new Value(Value::DECTYPE, new float(*val1 <= *val2), outError);
	case TokenType::BIG:
		return new Value(Value::DECTYPE, new float(*val1 > *val2), outError);
	case TokenType::BIGEQ:
		return new Value(Value::DECTYPE, new float(*val1 >= *val2), outError);
	case TokenType::AND:
		return new Value(Value::DECTYPE, new float(*val1 && *val2), outError);
	case TokenType::OR:
		return new Value(Value::DECTYPE, new float(*val1 || *val2), outError);
	case TokenType::MOD:
		if (val2 == 0) {
			RuntimeError targetError = RuntimeError(std::format("ZeroModError: {}/{}", std::to_string(*val1), "0"), 
				line, startColumn, endColumn);
			outError.copy(targetError);
			return nullptr;
		}
		return new Value(Value::DECTYPE, new float{ std::fmod(*val1, *val2) }, outError);
	}

	RuntimeError targetError = RuntimeError("Invalid Operator: ", line, startColumn, endColumn);
	outError.copy(targetError);
	return nullptr;
}

Value* Value::doOp(Value& other, enum TokenType op, Error& outError)
{
	if (data == nullptr) {
		RuntimeError targetError = RuntimeError("Cannot acces data of variable which is not defined", 
			line, startColumn, endColumn);
		outError.copy(targetError);
		return nullptr;
	}
	Value* otherData = other.run(outError);
	if (otherData == nullptr)
		return nullptr;

	void* castVal = Cast(otherData->data, otherData->dataType, this->dataType, outError);
	if (castVal == nullptr)
		return nullptr;

	switch (this->dataType)
	{
	case INTTYPE:
		return calcOp<int>((int*)data, (int*)castVal, op, outError);
	case DECTYPE:
		return calcOp<float>((float*)data, (float*)castVal, op, outError);
	default:
		RuntimeError targetError = RuntimeError("Invalid DataType", line, startColumn, endColumn);
		outError.copy(targetError);
		return nullptr;
	}
}

void* Value::getData() {
	return data;
}

std::ostream& operator<<(std::ostream& output, const Value& e)
{
	if(e.data == nullptr)
	{
		output << "NULL";
	}
	else {
		switch (e.dataType)
		{
		case Value::INTTYPE:
			output << *(int*)e.data;
			break;
		case Value::DECTYPE:
			output << *(float*)e.data;
			break;
		case Value::UNDEFINED:
			output << "undefined";
			break;
		default:
			break;
		}
	}
	return output;
}

void* Value::Cast(void* data, enum DataType o_type, enum DataType t_type, Error& outError) {
	// checks viable conversions
	void* returnValue{ nullptr };
	switch (t_type)
	{
	case INTTYPE:
		switch (o_type)
		{
		case INTTYPE:
			returnValue = (void*)new int{ *(int*)data };
			break;
		case DECTYPE:
			returnValue = (void*)new int(*(float*)data);
			break;
		}
		break;
	case DECTYPE:
		switch (o_type)
		{
		case INTTYPE:
			returnValue = (void*)new float(*(int*)data);
			break;
		case DECTYPE:
			returnValue = (void*)new float{ *(float*)data };
			break;
		}
		break;

	default:
		break;
	}
	if (returnValue == nullptr) {
		RuntimeError targetError = RuntimeError(std::format("Couldnt cast {} to {}", std::to_string(o_type),
			std::to_string(t_type)));
		return nullptr;
	}
	return returnValue;
}
