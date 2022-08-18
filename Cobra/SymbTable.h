#pragma once
#include <string>
#include <map>
#include <functional>
#include "SuppType.h"

class SymbTable
{
private:


public:
	SymbTable* parent;
	std::map<std::string, int> varReg;
	std::map<std::string, int> integers;
	std::map<std::string, float> floats;

	std::map<std::string, int> varNames;

	SymbTable(SymbTable* parent = nullptr);

	template<SuppType T>
	void declare(std::string name, T value);

	template<>
	void declare(std::string name, int value);

	template<>
	void declare(std::string name, float value);

	template<SuppType T>
	T run(std::string name);

	template<>
	float run(std::string name);

	template<>
	int run(std::string name);

	template<SuppType T>
	void set(std::string name, T value);

	template<>
	void set(std::string name, int value);

	template<>
	void set(std::string name, float value);

	void reg(std::string name, int type);

	void clearReg();
};

