#pragma once
#include <string>
#include <map>
#include <functional>
#include "SuppType.h"
#include "Error.h"
#include "Value.h"

class SymbTable
{
public:
	SymbTable* parent;
	std::map<std::string, Value*> variables;

	SymbTable(SymbTable* parent = nullptr);
	~SymbTable();

	void declare(std::string name, Value* val, Error& outError, bool isConst = false, bool isStaticType = false);
	void declare(std::string name, enum Value::DataType dataType, Error& outError, void* data = nullptr, bool isConst = false, bool isStaticType = false);
	Value* run(std::string name, Error& outError);
	void set(std::string name, Value* val, Error& outError);

	bool isVarDecl(std::string name);


#pragma region OLD-CODE
	//std::map<std::string, int> parseReg;
	//std::map<std::string, int> integers;
	//std::map<std::string, float> floats;

	//std::map<std::string, int> varNames;

	/*template<SuppType T>
	Error declare(std::string name, T value);

	template<>
	Error declare(std::string name, int value);

	template<>
	Error declare(std::string name, float value);

	template<SuppType T>
	std::pair<T, Error> run(std::string name);

	template<>
	std::pair<float, Error> run(std::string name);

	template<>
	std::pair<int, Error> run(std::string name);

	template<SuppType T>
	Error set(std::string name, T value);

	template<>
	Error set(std::string name, int value);

	template<>
	Error set(std::string name, float value);

	Error reg(std::string name, int type);*/

	//bool isVarReg(std::string name);

	//std::pair<int, Error> getRegVar(std::string name);

	//Error clearParseReg();

	//void clearVarNames();
#pragma endregion
};

