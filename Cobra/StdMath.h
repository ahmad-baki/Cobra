#pragma once
#include "Value.h"
#include "PackageManager.h"
#include <functional>
#include <map>

class StdMath
{
public:
	static std::map<std::string, cobrfunc_t> getFuncs();

	static std::vector<Value*> cobrAbs(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrFloor(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrCeil(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrRound(std::vector<std::vector<Value*>> param, Error& outError);
	
//private:
//	static std::map<std::string, cobrfunc_t> funcs;
};

