#pragma once
#include <map>
#include <functional>
#include "Value.h"
#include "Token.h"

typedef std::function<std::vector<Value*>(std::vector<std::vector<Value*>>, Error&)> cobrfunc_t;

class PackageManager
{
public:
	static std::map<std::string, cobrfunc_t> import(std::vector<Token> packages, Error& outError);

	// helper functions:
	static bool hasSize(std::vector<std::vector<Value*>> param, size_t size, Error& outError);
	static bool cast(std::vector<std::vector<Value*>>& param, std::vector<int> t_types, Error& outError);
	// should always be called when PackageManager::cast ist called
	static void deleteData(std::vector<std::vector<Value*>>& param);
};

