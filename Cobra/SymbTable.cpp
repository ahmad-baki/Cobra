#include "SymbTable.h"
#include <stdexcept>
#include "RuntimeError.h"
#include "SyntaxError.h"
#include <format>
#include "Variable.h"

#pragma region OLD-CODE
//SymbTable::SymbTable(SymbTable* parent) 
//	: parent{ parent }, variables{ std::map<std::string, Value*>() }
//{
//#pragma region OLD-CODE
//	//integers = std::map<std::string, int>();
//	//floats = std::map<std::string, float>();
//	//// 0=int, 1=float
//	//varNames = std::map<std::string, int>();
//	//parseReg = std::map<std::string, int>();
//#pragma endregion
//};
//
//SymbTable::~SymbTable()
//{
//	for (auto const& [key, val] : variables) {
//		delete val;
//	}
//}
//
//void SymbTable::declareVar(std::string name, Value* val, Error& outError, bool isConst, bool isStaticType) {
//	declareVar(name, val->getTypeId(), outError, val->getData(), isConst, isStaticType);
//}
//
//void SymbTable::declareVar(std::string name, int typeId, Error& outError, void* data, bool isConst, bool isStaticType) {
//	if (isVarDecl(name)) {
//		Error targetError(ErrorType::RUNTIMEERROR, "Tried to declar variable " + name + ", despite it already existing");
//		outError.copy(targetError);
//	}
//
//	Variable* newVal = new Variable{ typeId, data, outError, isConst, isStaticType };
//	variables.emplace(name, newVal);
//}
//
//void SymbTable::setVar(std::string name, Value* tVal, Error& outError)
//{
//	Value* val = this->run(name, outError);
//
//	if (val == nullptr)
//		return;
//
//	val->setVal(tVal, outError);
//	if (outError.errType != ErrorType::NULLERROR)
//		return;
//}
//
//Value* SymbTable::run(std::string name, Error& outError)
//{
//	auto elem = variables.find(name);
//	if (elem == variables.end()) {
//		if (parent != nullptr) {
//			parent->run(name, outError);
//		}
//
//		Error targetError(ErrorType::RUNTIMEERROR, "Tried to run variable " + name +", despite it not existing");
//		outError.copy(targetError);
//		return nullptr;
//	}
//	return elem->second;
//}
//
//bool SymbTable::isVarDecl(std::string name) {
//	auto elem = variables.find(name);
//	if (elem == variables.end())
//		return true;
//	if (parent != nullptr)
//		return parent->isVarDecl(name);
//	return false;
//}
//template<SuppType T>
//Error SymbTable::declareVar(std::string name, T value) {
//	if (isVarDecl(name)) {
//		return RuntimeError("Invalid type");
//	}
//}
//
//template<>
//Error SymbTable::declareVar(std::string name, bool value) {
//	return declareVar<int>(name, value);
//}
//
//template<>
//Error SymbTable::declareVar(std::string name, int value) {
//	if (isVarDecl(name)){
//		return RuntimeError("tried to declareVar " + name + " despite it already existing");
//	}
//	integers.insert(std::pair<std::string, int>(name, value));
//	varNames.insert(std::pair<std::string, int>(name, 0));
//	return Error();
//}
//
//template<>
//Error SymbTable::declareVar(std::string name, float value) {
//	if (isVarDecl(name)) {
//		return RuntimeError("tried to declareVar " + name + " despite it already existing");
//	}
//	floats.insert(std::pair<std::string, float>(name, value));
//	varNames.insert(std::pair<std::string, int>(name, 1));
//	return Error();
//}
//#pragma endregion
//
//
//
//#pragma region SET
//
//template<SuppType T>
//Error SymbTable::setVar(std::string name, T value) {
//	if (isVarDecl(name)) {
//		return RuntimeError("invalid type");
//	}
//	//throw std::invalid_argument("invalid type");
//	//auto elem = varNames.find(name);
//	//if (elem == varNames.end()) {
//	//	if(parent == nullptr)
//	//		(*parent).setVar(name, value);
//	//	else
//	//		throw std::invalid_argument("tried to setVar" + name + "despite it not existing");
//	//}
//	//switch ((*elem).second)
//	//{
//	//case 0:
//	//	integers[name] = value;
//	//	break;
//	//case 1:
//	//	floats[name] = value;
//	//	break;
//	//default:
//	//	throw std::invalid_argument("did you develop a 3. type?");
//	//}
//}
//
//template<>
//Error SymbTable::setVar(std::string name, bool value) {
//	return setVar<int>(name, value);
//}
//
//
//template<>
//Error SymbTable::setVar(std::string name, int value) {
//	auto elem = integers.find(name);
//	if (elem == integers.end()){	
//		if (parent == nullptr) {
//			return RuntimeError("tried to setVar " + name + " despite it not existing");
//		}
//
//		Error intError = parent->setVar<int>(name, value);
//		if (intError.m_errorName == "NULL")
//			return Error();
//
//		Error floatError = parent->setVar<float>(name, value);
//
//		if (floatError.m_errorName != "NULL")
//			return floatError;
//	}
//	else {
//		integers[name] = value;
//	}
//	return Error();
//}
//
//template<>
//Error SymbTable::setVar(std::string name, float value) {
//	auto elem = floats.find(name);
//	if (elem == floats.end()) {
//		if (parent == nullptr) {
//			return RuntimeError("tried to setVar " + name + " despite it not existing");
//		}
//
//		Error floatError = parent->setVar<float>(name, value);
//		if (floatError.m_errorName == "NULL")
//			return Error();
//
//		Error intError = parent->setVar<int>(name, value);
//
//		if (intError.m_errorName != "NULL")
//			return intError;
//	}
//	else {
//		floats[name] = value;
//	}
//	return Error();
//}
//#pragma endregion
//
//
//#pragma region GET
//
//template<SuppType T>
//std::pair<T, Error> SymbTable::run(std::string name) {
//	return RuntimeError("invalid type");
//	
//	//auto elem = varNames.find(name);
//	//if (elem == varNames.end()) {
//	//	if (parent == nullptr)
//	//		return (*parent).run<T>(name);
//	//	throw std::invalid_argument("tried to setVar" + name + "despite it not existing");
//	//}
//	//switch ((*elem).second)
//	//{
//	//case 0:
//	//	return (T)integers[name];
//	//case 1:
//	//	return (T)floats[name];
//	//default:
//	//	throw std::invalid_argument("did you develop a 3. type?");
//	//}
//}
//
//template<>
//std::pair<bool, Error> SymbTable::run(std::string name) {
//	return run<int>(name);
//}
//
//template<>
//std::pair<int, Error> SymbTable::run(std::string name) {
//	auto elem = integers.find(name);
//	if (elem == integers.end()) {
//		if (parent == nullptr) {
//			return { 0, RuntimeError("tried to run undeclared variable " + name) };
//		}
//
//		auto [intVal, intError] = parent->run<int>(name);
//
//		if (intError.m_errorName == "NULL")
//			return { intVal, Error() };
//
//		auto [floatVal, floatError] = parent->run<float>(name);
//
//		if (floatError.m_errorName == "NULL")
//			return { floatVal, Error() };
//
//		return { {}, floatError };
//	}
//	return { elem->second, Error() };
//}
//
//template<>
//std::pair<float, Error> SymbTable::run(std::string name) {
//	auto elem = floats.find(name);
//	if (elem == floats.end()) {
//
//		if (parent == nullptr) {
//			return { 0, RuntimeError("tried to run undeclared variable " + name) };
//		}
//
//		auto [floatVal, floatError] = parent->run<float>(name);
//
//		if (floatError.m_errorName == "NULL")
//			return { floatVal, Error() };
//
//		auto [intVal, intError] = parent->run<int>(name);
//
//		if (intError.m_errorName == "NULL")
//			return { intVal, Error() };
//
//		return { {}, intError };
//	}
//	return { elem->second, Error() };
//}
//
//#pragma endregion
//
//
//
//Error SymbTable::reg(std::string name, int type)
//{
//	if (isVarDecl(name))
//		return SyntaxError("tried to register already existing variablename");
//
//	parseReg.insert(std::pair<std::string, int>(name, type));
//
//	return Error();
//}
//
//}
//
//bool SymbTable::isVarReg(std::string name) {
//	if (parseReg.contains(name))
//		return true;
//	if (parent != nullptr)
//		return parent->isVarReg(name);
//	return false;
//}
//
//std::pair<int, Error> SymbTable::getRegVar(std::string name) {
//	auto elem = parseReg.find(name);
//	if (elem != parseReg.end())
//		return { elem->second, Error() };
//	if (parent != nullptr)
//		return parent->getRegVar(name);
//	return { 0, SyntaxError("tried to run not existing variable " + name) };
//}
//
//Error SymbTable::clearParseReg() {
//	parseReg.clear();
//	return Error();
//}
//
//void SymbTable::clearVariables() {
//	varNames.clear();
//	integers.clear();
//	floats.clear();
//}
#pragma endregion