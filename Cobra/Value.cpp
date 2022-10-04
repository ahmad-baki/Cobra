#include "Value.h"
#include "RuntimeError.h"
#include <format>
#include "TokenType.cpp"
#include <vector>

Value::Value()
	: dataType{ UNDEFINED }, data{ nullptr }, isConst{ false }, 
	isStaticType{ false }, constrError( Error() ), isIter{ false }
{
}

Value::Value(enum DataType dataType, void* data, bool isConst, bool isStaticType, bool isIter)
	: dataType{ dataType }, data{ data }, isConst{ isConst }, 
	isStaticType{ isStaticType }, constrError{ Error() }, isIter{ isIter }
{
	if (data == nullptr || dataType == -1) {
		this->dataType = dataType;
		this->data = nullptr;
		return;
	}
	auto [castVal, castError] = Cast(data, dataType, this->dataType);

	if (castError.m_errorName != "NULL") {
		constrError = castError;
		return;
	}

	this->dataType = dataType;
	this->data = castVal;
}

Value::Value(Value* other, enum DataType dataType, bool isConst, bool isStaticType, bool isIter)
	: isConst{ isConst }, isStaticType{ isStaticType }, constrError{ Error() }, isIter{ isIter }
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
	auto [castVal, castError] = Cast(other->data, dataType, this->dataType);

	if (castError.m_errorName != "NULL") {
		constrError = castError;
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
	isIter			= old_obj.isIter;
}

Value::~Value() {
	if (isIter) {
		std::vector<Value*> arr = *(std::vector<Value*>*)data;
		for (Value* valP : arr) {
			delete valP;
		}

	}
	delete data;
}

std::pair<Value*, Error> Value::run() {
	return { this, Error() };
}

Error Value::setVal(Value* val) {
	if (isConst) {
		return RuntimeError("Tried to write in const Variable");
	}
	if (this->dataType == UNDEFINED) {
		if(data != nullptr)
			delete this->data;
		this->data = val->getData();
		this->dataType = val->getType();
	}
	else if (isStaticType && dataType != val->getType()) {
		auto [castVal, castError] = Cast(val->getData(), val->getType(), this->dataType);
		if (castError.m_errorName != "NULL")
			return castError;

		delete this->data;
		this->data = castVal;
	}
	else {
		if (data != nullptr)
			delete this->data;
		this->data = val->getData();
		this->dataType = val->getType();
	}
	return Error();
}

std::pair<bool, Error> Value::getBool()
{
	switch (dataType)
	{
	case 0:
		return { bool(*(int*)data), Error() };
	case 1:
		return { bool(*(float*)data), Error() };
	case -1:
		return { false, RuntimeError("Cannot acces data of variable which is not defined")};
	default:
		return { false, RuntimeError("Invalid DataType") };
	}
}

Value::DataType Value::getType()
{
	return dataType;
}


template<>
std::pair<Value*, Error> Value::calcOp(int* val1, int* val2, enum TokenType op) {
	switch (op)
	{
	case TokenType::PLUS:
		return { new Value(Value::INTTYPE, new int{*val1 + *val2}), Error() };
	case TokenType::MINUS:
		return { new Value(Value::INTTYPE, new int{*val1 - *val2}), Error() };
	case TokenType::DIV:
		if (*val2 == 0) {
			return { nullptr,
				RuntimeError(std::format("ZeroDivisionError: {}/{}", std::to_string(*val1), "0"))
			};
		}
		return { new Value(Value::INTTYPE, new int{*val1 / *val2}), Error() };
	case TokenType::MUL:
		return { new Value(Value::INTTYPE, new int{*val1 * *val2}), Error() };
	case TokenType::EQEQ:
		return { new Value(Value::INTTYPE, new int{*val1 == *val2}), Error() };
	case TokenType::EXCLAEQ:
		return { new Value(Value::INTTYPE, new int{*val1 != *val2}), Error() };
	case TokenType::SMALL:
		return { new Value(Value::INTTYPE, new int{*val1 < *val2}), Error() };
	case TokenType::SMALLEQ:
		return { new Value(Value::INTTYPE, new int{*val1 <= *val2}), Error() };
	case TokenType::BIG:
		return { new Value(Value::INTTYPE, new int{*val1 > *val2}), Error() };
	case TokenType::BIGEQ:
		return { new Value(Value::INTTYPE, new int{*val1 >= *val2}), Error() };
	case TokenType::AND:
		return { new Value(Value::INTTYPE, new int{*val1 && *val2}), Error() };
	case TokenType::OR:
		return { new Value(Value::INTTYPE, new int{*val1 || *val2}), Error() };
	case TokenType::MOD:
		if (val2 == 0) {
			return { {},
				RuntimeError(std::format("ZeroModError: {}/{}", std::to_string(*val1), "0"))
			};
		}
		return { new Value(Value::INTTYPE, new int(std::fmod(*val1, *val2))), Error() };
	}

	return { nullptr,
		RuntimeError(std::format("Invalid Operator: {}", std::to_string(op)))
	};//, this->line, this->startColumn, this->endColumn
}

template<>
std::pair<Value*, Error> Value::calcOp(float* val1, float* val2, enum TokenType op) {
	switch (op)
	{
	case TokenType::PLUS:
		return { new Value(Value::DECTYPE, new float{*val1 + *val2}), Error() };
	case TokenType::MINUS:
		return { new Value(Value::DECTYPE, new float{*val1 - *val2}), Error() };
	case TokenType::DIV:
		if (val2 == 0) {
			return { nullptr,
				RuntimeError(std::format("ZeroDivisionError: {}/{}", std::to_string(*val1), '0'))
			};
		}
		return { new Value(Value::DECTYPE, new float{*val1 / *val2}), Error() };
	case TokenType::MUL:
		return { new Value(Value::DECTYPE, new float{*val1 * *val2}), Error() };
	case TokenType::EQEQ:
		return { new Value(Value::DECTYPE, new float(*val1 == *val2)), Error() };
	case TokenType::EXCLAEQ:
		return { new Value(Value::DECTYPE, new float(*val1 != *val2)), Error() };
	case TokenType::SMALL:
		return { new Value(Value::DECTYPE, new float(*val1 < *val2)), Error() };
	case TokenType::SMALLEQ:
		return { new Value(Value::DECTYPE, new float(*val1 <= *val2)), Error() };
	case TokenType::BIG:
		return { new Value(Value::DECTYPE, new float(*val1 > *val2)), Error() };
	case TokenType::BIGEQ:
		return { new Value(Value::DECTYPE, new float(*val1 >= *val2)), Error() };
	case TokenType::AND:
		return { new Value(Value::DECTYPE, new float(*val1 && *val2)), Error() };
	case TokenType::OR:
		return { new Value(Value::DECTYPE, new float(*val1 || *val2)), Error() };
	case TokenType::MOD:
		if (val2 == 0) {
			return { {},
				RuntimeError(std::format("ZeroModError: {}/{}", std::to_string(*val1), "0"))
			};
		}
		return { new Value(Value::DECTYPE, new float{std::fmod(*val1, *val2)}), Error() };
	}

	return { nullptr,
		RuntimeError("Invalid Operator: ")
	};//, this->line, this->startColumn, this->endColumn
}

std::pair<Value*, Error> Value::doOp(Value& other, enum TokenType op)
{
	if(data == nullptr)
		return { nullptr, RuntimeError("Cannot acces data of variable which is not defined") }
	;
	auto [otherData, runError] = other.run();
	if (runError.m_errorName != "NULL")
		return { nullptr, runError };

	auto [castVal, castError] = Cast(otherData->data, otherData->dataType, this->dataType);
	if (castError.m_errorName != "NULL")
		return { nullptr, castError };

	switch (this->dataType)
	{
	case INTTYPE:
		return calcOp<int>((int*)data, (int*)castVal, op);
	case DECTYPE:
		return calcOp<float>((float*)data, (float*)castVal, op);
	default:
		return { nullptr, RuntimeError("Invalid DataType") };
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


std::pair<void*, Error> Value::Cast(void* data, enum DataType o_type, enum DataType t_type) {
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
		return { nullptr, RuntimeError(std::format("Couldnt cast {} to {}", std::to_string(o_type), std::to_string(t_type))) };
	}
	return { returnValue, Error()};
}
