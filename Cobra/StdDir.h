#pragma once
#include <map>
#include <string>
#include "PackageManager.h"

class StdDir
{
public:
	static std::map<std::string, cobrfunc_t> getFuncs();
	static std::vector<Value*> createDir(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> writeFile(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> existPath(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> getElemInDir(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> getFile(std::vector<std::vector<Value*>> param, Error& outError);

};

