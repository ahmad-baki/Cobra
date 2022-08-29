#include "SymbTable.h"
#include <stdexcept>
#include "RuntimeError.h"
#include "SyntaxError.h"

SymbTable::SymbTable(SymbTable* parent) : parent{ parent }{
	integers = std::map<std::string, int>();
	floats = std::map<std::string, float>();
	// 0=int, 1=float
	varNames = std::map<std::string, int>();
	parseReg = std::map<std::string, int>();
};

#pragma region DECLARATION

template<SuppType T>
Error SymbTable::declare(std::string name, T value) {
	if (isVarDecl(name)) {
		return RuntimeError("Invalid type");
	}
}

template<>
Error SymbTable::declare(std::string name, bool value) {
	return declare<int>(name, value);
}

template<>
Error SymbTable::declare(std::string name, int value) {
	if (isVarDecl(name)){
		return RuntimeError("tried to declare " + name + " despite it already existing");
	}
	integers.insert(std::pair<std::string, int>(name, value));
	varNames.insert(std::pair<std::string, int>(name, 0));
	return Error();
}

template<>
Error SymbTable::declare(std::string name, float value) {
	if (isVarDecl(name)) {
		return RuntimeError("tried to declare " + name + " despite it already existing");
	}
	floats.insert(std::pair<std::string, float>(name, value));
	varNames.insert(std::pair<std::string, int>(name, 1));
	return Error();
}
#pragma endregion



#pragma region SET

template<SuppType T>
Error SymbTable::set(std::string name, T value) {
	if (isVarDecl(name)) {
		return RuntimeError("invalid type");
	}
	//throw std::invalid_argument("invalid type");
	//auto elem = varNames.find(name);
	//if (elem == varNames.end()) {
	//	if(parent == nullptr)
	//		(*parent).set(name, value);
	//	else
	//		throw std::invalid_argument("tried to set" + name + "despite it not existing");
	//}
	//switch ((*elem).second)
	//{
	//case 0:
	//	integers[name] = value;
	//	break;
	//case 1:
	//	floats[name] = value;
	//	break;
	//default:
	//	throw std::invalid_argument("did you develop a 3. type?");
	//}
}

template<>
Error SymbTable::set(std::string name, bool value) {
	return set<int>(name, value);
}


template<>
Error SymbTable::set(std::string name, int value) {
	auto elem = integers.find(name);
	if (elem == integers.end()){	
		if (parent == nullptr) {
			return RuntimeError("tried to set " + name + " despite it not existing");
		}

		Error intError = parent->set<int>(name, value);
		if (intError.m_errorName == "NULL")
			return Error();

		Error floatError = parent->set<float>(name, value);

		if (floatError.m_errorName != "NULL")
			return floatError;
	}
	else {
		integers[name] = value;
	}
	return Error();
}

template<>
Error SymbTable::set(std::string name, float value) {
	auto elem = floats.find(name);
	if (elem == floats.end()) {
		if (parent == nullptr) {
			return RuntimeError("tried to set " + name + " despite it not existing");
		}

		Error floatError = parent->set<float>(name, value);
		if (floatError.m_errorName == "NULL")
			return Error();

		Error intError = parent->set<int>(name, value);

		if (intError.m_errorName != "NULL")
			return intError;
	}
	else {
		floats[name] = value;
	}
	return Error();
}
#pragma endregion


#pragma region GET

template<SuppType T>
std::pair<T, Error> SymbTable::run(std::string name) {
	return RuntimeError("invalid type");
	
	//auto elem = varNames.find(name);
	//if (elem == varNames.end()) {
	//	if (parent == nullptr)
	//		return (*parent).run<T>(name);
	//	throw std::invalid_argument("tried to set" + name + "despite it not existing");
	//}
	//switch ((*elem).second)
	//{
	//case 0:
	//	return (T)integers[name];
	//case 1:
	//	return (T)floats[name];
	//default:
	//	throw std::invalid_argument("did you develop a 3. type?");
	//}
}

template<>
std::pair<bool, Error> SymbTable::run(std::string name) {
	return run<int>(name);
}

template<>
std::pair<int, Error> SymbTable::run(std::string name) {
	auto elem = integers.find(name);
	if (elem == integers.end()) {
		if (parent == nullptr) {
			return { 0, RuntimeError("tried to get undeclared variable " + name) };
		}

		auto [intVal, intError] = parent->run<int>(name);

		if (intError.m_errorName == "NULL")
			return { intVal, Error() };

		auto [floatVal, floatError] = parent->run<float>(name);

		if (floatError.m_errorName == "NULL")
			return { floatVal, Error() };

		return { {}, floatError };
	}
	return { elem->second, Error() };
}

template<>
std::pair<float, Error> SymbTable::run(std::string name) {
	auto elem = floats.find(name);
	if (elem == floats.end()) {

		if (parent == nullptr) {
			return { 0, RuntimeError("tried to get undeclared variable " + name) };
		}

		auto [floatVal, floatError] = parent->run<float>(name);

		if (floatError.m_errorName == "NULL")
			return { floatVal, Error() };

		auto [intVal, intError] = parent->run<int>(name);

		if (intError.m_errorName == "NULL")
			return { intVal, Error() };

		return { {}, intError };
	}
	return { elem->second, Error() };
}

#pragma endregion



Error SymbTable::reg(std::string name, int type)
{
	if (isVarDecl(name))
		return SyntaxError("tried to register already existing variablename");

	parseReg.insert(std::pair<std::string, int>(name, type));

	return Error();
}

bool SymbTable::isVarDecl(std::string name) {
	if (varNames.contains(name))
		return true;
	if (parent != nullptr)
		return parent->isVarDecl(name);
	return false;
}

bool SymbTable::isVarReg(std::string name) {
	if (parseReg.contains(name))
		return true;
	if (parent != nullptr)
		return parent->isVarReg(name);
	return false;
}

std::pair<int, Error> SymbTable::getRegVar(std::string name) {
	auto elem = parseReg.find(name);
	if (elem != parseReg.end())
		return { elem->second, Error() };
	if (parent != nullptr)
		return parent->getRegVar(name);
	return { 0, SyntaxError("tried to get not existing variable " + name) };
}

Error SymbTable::clearParseReg() {
	parseReg.clear();
	return Error();
}

void SymbTable::clearVarNames() {
	varNames.clear();
	integers.clear();
	floats.clear();
}