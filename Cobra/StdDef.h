#pragma once
#include "PackageManager.h"

class StdDef
{
public:
	static std::map<std::string, cobrfunc_t> getFuncs();

	static std::vector<Value*> input(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> print(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> printLi(std::vector<std::vector<Value*>> param, Error& outError);
};

