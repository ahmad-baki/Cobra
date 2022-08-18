#include "SymbTable.h"
#include <stdexcept>

SymbTable::SymbTable(SymbTable* parent) : parent{ parent }{
	integers = std::map<std::string, int>();
	floats = std::map<std::string, float>();
	// 0=int, 1=float
	varNames = std::map<std::string, int>();
	varReg = std::map<std::string, int>();
};

#pragma region DECLARATION

template<SuppType T>
void SymbTable::declare(std::string name, T value) {
	throw std::exception("invalid type");
}

template<>
void SymbTable::declare(std::string name, bool value) {
	declare<int>(name, value);
}

template<>
void SymbTable::declare(std::string name, int value) {
	auto elem = varNames.find(name);
	if (elem != varNames.end())
		throw std::invalid_argument("tried to declare" + name + "despite it already existing");
	integers.insert(std::pair<std::string, int>(name, value));
	varNames.insert(std::pair<std::string, int>(name, 0));
}

template<>
void SymbTable::declare(std::string name, float value) {
	auto elem = floats.find(name);
	if (elem != floats.end())
		throw std::invalid_argument("tried to declare" + name + "despite it already existing");
	floats.insert(std::pair<std::string, int>(name, value));
	varNames.insert(std::pair<std::string, int>(name, 1));
}
#pragma endregion



#pragma region SET

template<SuppType T>
void SymbTable::set(std::string name, T value) {
	throw std::exception("invalid type");
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
void SymbTable::set(std::string name, bool value) {
	set<int>(name, value);
}


template<>
void SymbTable::set(std::string name, int value) {
	auto elem = integers.find(name);
	if (elem == integers.end())
		// then checks floats

		(*parent).set(name, value);
	else {
		integers[name] = value;
	}
}

template<>
void SymbTable::set(std::string name, float value) {
	auto elem = floats.find(name);
	if (elem == floats.end())
		(*parent).set(name, value);
	floats[name] = value;
}

void SymbTable::reg(std::string name, int type)
{
	varReg.insert(std::pair<std::string, int>(name, type));
}

#pragma endregion


#pragma region GET

template<SuppType T>
T SymbTable::run(std::string name) {
	throw std::exception("invalid type");
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
bool SymbTable::run(std::string name) {
	return run<int>(name);
}

template<>
int SymbTable::run(std::string name) {
	auto elem = integers.find(name);
	if (elem == integers.end()) {
		return (*parent).run<int>(name);
	}
	return (*elem).second;
}

template<>
float SymbTable::run(std::string name) {
	auto elem = floats.find(name);
	if (elem == floats.end()) {
		return (*parent).run<float>(name);
	}
	return (*elem).second;
}

#pragma endregion

void SymbTable::clearReg() {
	varReg.clear();
}