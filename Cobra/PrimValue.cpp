#include "PrimValue.h"
#include "RuntimeError.h"
#include <format>
#include "TokenType.cpp"
#include <vector>
#include <string>
#include <sstream>
#include "Interpreter.h"



//PrimValue::PrimValue()
//{
//	typeId{ 0 };
//	data{ nullptr };
//}

PrimValue::PrimValue(int typeId, void* data, Error& outError)
{
	this->typeId = typeId;
	this->data = data;
}

PrimValue::PrimValue(Value* other, Error& outError, int typeId)
{
	if (other == nullptr) {
		this->typeId = typeId;
		this->data = nullptr;
	}
	else {
		if (typeId == 0) {
			this->typeId = other->getTypeId();
		}
		else {
			this->typeId = typeId;
		}

		void* castVal = Cast(other->getData(), other->getTypeId(), this->typeId, outError);
		if (castVal == nullptr)
			return;

		if (this->data != nullptr) {
			delete this->data;
		}
		this->data = castVal;
	}
}

PrimValue::~PrimValue() {
	if (data != nullptr) {
		delete data;
	}
	data = nullptr;
}

std::vector<Value*> PrimValue::run(Error& outError) {
	return { this };
}

//void PrimValue::setVal(Value* val, Error& outError) {
//	void* castVal = Cast(val->getData(), val->getTypeId(), this->typeId, outError);
//	if (castVal == nullptr)
//		return;
//
//	if (this->data != nullptr) {
//		delete this->data;
//	}
//	this->data = castVal;
//}

bool PrimValue::getBool(Error& outError)
{
	Error targetError{};
	Interpreter* interpr = Interpreter::getSingelton();
	if (typeId == interpr->getTypeId("int", outError)) {
		return bool(*(int*)data);
	}
	if (typeId == interpr->getTypeId("float", outError)) {
		return bool(*(float*)data);
	}
	if (typeId == 0) {
		targetError = Error(ErrorType::RUNTIMEERROR, "Cannot acces data of variable which is not defined",
			line, startColumn, endColumn);
		outError.copy(targetError);
		return false;
	}

	Type* type = interpr->getType(typeId, outError);
	if (type == nullptr) {
		return false;
	}
	std::string typeName = type->getTypeName();

	targetError = Error(ErrorType::RUNTIMEERROR, std::format("Invalid DataType: {}", typeName), line, startColumn, endColumn);
	outError.copy(targetError);
	return false;
}

int PrimValue::getType()
{
	return typeId;
}

template<SuppType T>
PrimValue* PrimValue::calcOp(T* val1, T* val2, enum TokenType op, int typeId, Error& outError) {
	switch (op)
	{
	case TokenType::PLUS:
		return new PrimValue(typeId, new T( *val1 + *val2 ), outError);
	case TokenType::MINUS:
		return new PrimValue(typeId, new T( *val1 - *val2 ), outError);
	case TokenType::DIV:
		if (*val2 == 0) {
			Error targetError(ErrorType::RUNTIMEERROR, "ZeroDivisionError: " + 
				std::to_string(*val1) + "/0 ", line, startColumn, endColumn);
			outError.copy(targetError);
			return nullptr;
		}
		return new PrimValue(typeId, new T( *val1 / *val2 ), outError);
	case TokenType::MUL:
		return new PrimValue(typeId, new T( *val1 * *val2 ), outError);
	case TokenType::EQEQ:
		return new PrimValue(typeId, new T( *val1 == *val2 ), outError);
	case TokenType::EXCLAEQ:
		return new PrimValue(typeId, new T( *val1 != *val2 ), outError);
	case TokenType::SMALL:
		return new PrimValue(typeId, new T( *val1 < *val2 ), outError);
	case TokenType::SMALLEQ:
		return new PrimValue(typeId, new T( *val1 <= *val2 ), outError);
	case TokenType::BIG:
		return new PrimValue(typeId, new T( *val1 > *val2 ), outError);
	case TokenType::BIGEQ:
		return new PrimValue(typeId, new T( *val1 >= *val2 ), outError);
	case TokenType::AND:
		return new PrimValue(typeId, new T( *val1 && *val2 ), outError);
	case TokenType::OR:
		return new PrimValue(typeId, new T( *val1 || *val2 ), outError);
	case TokenType::MOD:
		if (val2 == 0) {
			Error targetError(ErrorType::RUNTIMEERROR, "ZeroModError: " + 
				std::to_string(*val1) + "/0", line, startColumn, endColumn);
			return nullptr;
		}
		return new PrimValue(typeId, new T(std::fmod(*val1, *val2)), outError);
	}

	Error targetError(ErrorType::RUNTIMEERROR, "Invalid Operator: " + std::to_string(op),
		line, startColumn, endColumn);
	outError.copy(targetError);
	return nullptr;
}

PrimValue* PrimValue::calcOp(std::string* val1, std::string* val2, enum TokenType op, int typeId, Error& outError) {
	switch (op)
	{
	case TokenType::PLUS:
		return new PrimValue(typeId, new std::string(*val1 + *val2), outError);
	case TokenType::EQEQ:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 == *val2)), outError);
	case TokenType::EXCLAEQ:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 != *val2)), outError);
	case TokenType::SMALL:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 < *val2)), outError);
	case TokenType::SMALLEQ:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 <= *val2)), outError);
	case TokenType::BIG:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 > *val2)), outError);
	case TokenType::BIGEQ:
		return new PrimValue(typeId, new std::string(std::to_string(*val1 >= *val2)), outError);
	case TokenType::AND:
		return new PrimValue(typeId, new std::string(std::to_string((*val1 != "0") && (*val2 != "0"))), outError);
	case TokenType::OR:
		return new PrimValue(typeId, new std::string(std::to_string((*val1 != "0") || (*val2 != "0"))), outError);
	default:
		break;
	}

	Error targetError(ErrorType::RUNTIMEERROR, "Invalid Operator: " + std::to_string(op),
		line, startColumn, endColumn);
	outError.copy(targetError);
	return nullptr;
}

////template<>
//Value* PrimValue::calcOp(float* val1, float* val2, enum TokenType op, Error& outError) {
//	switch (op)
//	{
//	case TokenType::PLUS:
//		return new PrimValue(PrimValue::DECTYPE, new float{*val1 + *val2}, outError);
//	case TokenType::MINUS:
//		return new PrimValue(PrimValue::DECTYPE, new float{ *val1 - *val2 }, outError);
//	case TokenType::DIV:
//		if (val2 == 0) {
//			Error targetError(ErrorType::RUNTIMEERROR, "ZeroDivisionError: " + std::to_string(*val1) + "/0",
//				line, startColumn, endColumn);
//			outError.copy(targetError);
//			return nullptr;
//		}
//		return new PrimValue(PrimValue::DECTYPE, new float{ *val1 / *val2 }, outError);
//	case TokenType::MUL:
//		return new PrimValue(PrimValue::DECTYPE, new float{ *val1 * *val2 }, outError);
//	case TokenType::EQEQ:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 == *val2), outError);
//	case TokenType::EXCLAEQ:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 != *val2), outError);
//	case TokenType::SMALL:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 < *val2), outError);
//	case TokenType::SMALLEQ:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 <= *val2), outError);
//	case TokenType::BIG:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 > *val2), outError);
//	case TokenType::BIGEQ:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 >= *val2), outError);
//	case TokenType::AND:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 && *val2), outError);
//	case TokenType::OR:
//		return new PrimValue(PrimValue::DECTYPE, new float(*val1 || *val2), outError);
//	case TokenType::MOD:
//		if (val2 == 0) {
//			Error targetError(ErrorType::RUNTIMEERROR, "ZeroModError: " + std::to_string(*val1) +"/{}",
//				line, startColumn, endColumn);
//			outError.copy(targetError);
//			return nullptr;
//		}
//		return new PrimValue(PrimValue::DECTYPE, new float{ std::fmod(*val1, *val2) }, outError);
//	}
//
//	Error targetError(ErrorType::RUNTIMEERROR, "Invalid Operator: ", line, startColumn, endColumn);
//	outError.copy(targetError);
//	return nullptr;
//}

Value* PrimValue::doOp(Value& other, enum TokenType op, Error& outError)
{
	if (data == nullptr) {
		Error targetError(ErrorType::RUNTIMEERROR, "Cannot acces data of variable which is not defined", 
			line, startColumn, endColumn);
		outError.copy(targetError);
		return nullptr;
	}

	void* castVal1;
	void* castVal2;
	Value* result{ nullptr };

	Interpreter* interpreter = Interpreter::getSingelton();
	int boolTypeId = interpreter->getTypeId("bool", outError);
	int intTypeId = interpreter->getTypeId("int", outError);
	int floatTypeId = interpreter->getTypeId("float", outError);
	int charTypeId = interpreter->getTypeId("char", outError);
	int stringTypeId = interpreter->getTypeId("string", outError);

	if (this->getTypeId() == floatTypeId || other.getTypeId() == floatTypeId) {
		castVal1 = Cast(this->getData(), this->getTypeId(), floatTypeId, outError);
		if (castVal1 == nullptr)
			return nullptr;
		castVal2 = Cast(other.getData(), other.getTypeId(), floatTypeId, outError);
		if (castVal2 == nullptr)
			return nullptr;
		result = calcOp((float*)castVal1, (float*)castVal2, op, floatTypeId, outError);
	}
	else if (this->getTypeId() == intTypeId || other.getTypeId() == intTypeId) {
		castVal1 = Cast(this->getData(), this->getTypeId(), intTypeId, outError);
		if (castVal1 == nullptr)
			return nullptr;
		castVal2 = Cast(other.getData(), other.getTypeId(), intTypeId, outError);
		if (castVal2 == nullptr)
			return nullptr;
		result = calcOp((int*)castVal1, (int*)castVal2, op, intTypeId, outError);
	}
	else if (this->getTypeId() == charTypeId || other.getTypeId() == charTypeId) {
		castVal1 = Cast(this->getData(), this->getTypeId(), charTypeId, outError);
		if (castVal1 == nullptr)
			return nullptr;
		castVal2 = Cast(other.getData(), other.getTypeId(), charTypeId, outError);
		if (castVal2 == nullptr)
			return nullptr;
		result = calcOp((char*)castVal1, (char*)castVal2, op, charTypeId, outError);
	}
	else if (this->getTypeId() == boolTypeId || other.getTypeId() == boolTypeId) {
		castVal1 = Cast(this->getData(), this->getTypeId(), boolTypeId, outError);
		if (castVal1 == nullptr)
			return nullptr;
		castVal2 = Cast(other.getData(), other.getTypeId(), boolTypeId, outError);
		if (castVal2 == nullptr)
			return nullptr;
		result = calcOp((bool*)castVal1, (bool*)castVal2, op, boolTypeId, outError);
	}
	else if (this->getTypeId() == stringTypeId || other.getTypeId() == stringTypeId) {
		castVal1 = Cast(this->getData(), this->getTypeId(), stringTypeId, outError);
		if (castVal1 == nullptr)
			return nullptr;
		castVal2 = Cast(other.getData(), other.getTypeId(), stringTypeId, outError);
		if (castVal2 == nullptr)
			return nullptr;
		result = calcOp((std::string*)castVal1, (std::string*)castVal2, op, stringTypeId, outError);
	}
	else{
		Error targetError(ErrorType::RUNTIMEERROR, "Invalid DataType", line, startColumn, endColumn);
		outError.copy(targetError);
		return nullptr;
	}
	delete castVal1;
	delete castVal2;
	return result;
}

void* PrimValue::getData() {
	return data;
}

Value* PrimValue::getCopy(int typeId, Error& outError)
{
	return new PrimValue(this, outError, typeId);
}


void* PrimValue::Cast(void* data, int o_typeId, int t_typeId, Error& outError) {

	if (t_typeId == -1) {
		Error targetError(ErrorType::RUNTIMEERROR, "Cant cast to void");
		return nullptr;
	}

	// checks viable conversions
	void* returnValue{ nullptr };
	Interpreter* interpreter = Interpreter::getSingelton();
	int boolTypeId = interpreter->getTypeId("bool", outError);
	int intTypeId = interpreter->getTypeId("int", outError);
	int floatTypeId = interpreter->getTypeId("float", outError);
	int charTypeId = interpreter->getTypeId("char", outError);
	int stringTypeId = interpreter->getTypeId("string", outError);


	if (t_typeId == boolTypeId) {
		if (o_typeId == boolTypeId) {
			returnValue = (void*)new bool(*(bool*)data);
		}
		if (o_typeId == intTypeId) {
			returnValue = (void*)new bool(*(int*)data);
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new bool{ *(float*)data != 0};
		}
		else if (o_typeId == charTypeId) {
			returnValue = (void*)new bool(*(char*)data);
		}
		else if (o_typeId == stringTypeId) {
			if (!isDec(*((std::string*)data))) {
				Error targetError(ErrorType::RUNTIMEERROR, std::format("Cant cast following string to bool: {}",
					*((std::string*)data)));
				outError.copy(targetError);
				return nullptr;
			}
			returnValue = (void*) new bool(std::stoi(*(std::string*)data));
		}
	}
	if (t_typeId == intTypeId) {
		if (o_typeId == boolTypeId) {
			returnValue = (void*)new int{ *(bool*)data };
		}
		else if (o_typeId == intTypeId){
			returnValue = (void*)new int{ *(int*)data };
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new int(*(float*)data);
		}
		else if (o_typeId == charTypeId) {
			returnValue = (void*)new int(*(char*)data);
		}
		else if (o_typeId == stringTypeId) {
			if (!isNumber(*((std::string*)data))) {
				Error targetError(ErrorType::RUNTIMEERROR, std::format("Cant cast following string to int: {}",
					*((std::string*)data)));
				outError.copy(targetError);
				return nullptr;
			}
			returnValue = (void*) new int(std::stof(*(std::string*)data));
		}
	}
	else if (t_typeId == floatTypeId) {
		if (o_typeId == boolTypeId) {
			returnValue = (void*)new float(*(bool*)data);
		}
		if (o_typeId == intTypeId) {
			returnValue = (void*)new float(*(int*)data);
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new float{ *(float*)data };
		}
		else if (o_typeId == charTypeId) {
			returnValue = (void*)new float(*(char*)data);
		}
		else if (o_typeId == stringTypeId) {
			if (!isDec(*((std::string*)data))) {
				Error targetError(ErrorType::RUNTIMEERROR, std::format("Cant cast following string to float: {}",
					*((std::string*)data)));
				outError.copy(targetError);
				return nullptr;
			}
			returnValue = (void*) new float(std::stof(*(std::string*)data));
		}
	}
	else if (t_typeId == charTypeId) {
		if (o_typeId == boolTypeId) {
			returnValue = (void*)new char(*(bool*)data);
		}
		if (o_typeId == intTypeId) {
			returnValue = (void*)new char(*(int*)data);
		}
		else if (o_typeId == charTypeId) {
			returnValue = (void*)new char(*(char*)data);
		}
		else if (o_typeId == stringTypeId) {
			if (((std::string*)data)->size() != 1) {
				Error targetError(ErrorType::RUNTIMEERROR, std::format("Cant cast following string to char: {}",
					*((std::string*)data)));
				outError.copy(targetError);
				return nullptr;
			}
			returnValue = (void*)new char((*(std::string*)data)[0]);
		}
	}
	else if (t_typeId == stringTypeId) {
		if (o_typeId == boolTypeId) {
			returnValue = (void*)new std::string(std::to_string(*(bool*)data));
		}
		if (o_typeId == intTypeId) {
			returnValue = (void*)new std::string(std::to_string(*(int*)data));
		}
		else if (o_typeId == floatTypeId) {
			returnValue = (void*)new std::string(std::to_string(*(float*)data));
		}
		else if (o_typeId == charTypeId) {
			returnValue = (void*)new std::string(std::to_string(* (char*)data));
		}
		else if (o_typeId == stringTypeId) {
			returnValue = (void*) new std::string(*(std::string*)data);
		}
	}

	if (returnValue == nullptr) {
		Error targetError(ErrorType::RUNTIMEERROR, std::format("Couldnt cast {} to {}", 
			Interpreter::getSingelton()->getType(o_typeId, outError)->getTypeName(), 
			Interpreter::getSingelton()->getType(t_typeId, outError)->getTypeName())
		);
		outError.copy(targetError);
		return nullptr;
	}
	return returnValue;
}

std::string PrimValue::toString()
{
	std::stringstream output;
	if (data == nullptr)
	{
		output << "NULL";
	}
	else {
		Error outError{};
		Interpreter* interpr = Interpreter::getSingelton();
		if (typeId == interpr->getTypeId("bool", outError)) {
			output << std::boolalpha;
			output << *(bool*)data;
			output << std::noboolalpha;
		}
		else if		(typeId == interpr->getTypeId("int", outError)) {
			output << *(int*)data;
		}
		else if (typeId == interpr->getTypeId("float", outError)) {
			output << *(float*)data;
		}
		else if (typeId == interpr->getTypeId("char", outError)) {
			output << *(char*)data;
		}
		else if (typeId == interpr->getTypeId("string", outError)) {
			output << *(std::string*)data;
		}
		else if (typeId == 0) {
			output << "undefined";
		}
	}
	return output.str();
}

bool PrimValue::isNumber(const std::string& s)
{
	return !s.empty() && 
		std::find_if(s.begin(), s.end(), 
			[](unsigned char c) { 
				return !std::isdigit(c); 
			}
		) == s.end();
}

bool PrimValue::isDec(const std::string& s)
{
	bool foundPoint = false;
	for (auto i = s.begin(); i != s.end(); i++) {
		if (!isdigit(*i)) {
			if (*i == '.') {
				if (foundPoint) {
					return false;
				}
				foundPoint = true;
			}
			return false;
		}
	}
	return true;
}