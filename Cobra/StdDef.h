#pragma once
#include "PackageManager.h"

class StdDef
{
public:
	static std::map<std::string, cobrfunc_t> getFuncs();

	static std::vector<Value*> input(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> print(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> printLi(std::vector<std::vector<Value*>> param, Error& outError);

	static std::vector<Value*> toBool(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> toInt(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> toFloat(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> toChar(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> toString(std::vector<std::vector<Value*>> param, Error& outError);

	static std::vector<Value*> elemAt(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> append(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> removeAt(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> slice(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> size(std::vector<std::vector<Value*>> param, Error& outError);

	static std::vector<Value*> strElemAt(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> strAppend(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> strRemoveAt(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> subStr(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> strSize(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> strMatch(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> charArrToStr(std::vector<std::vector<Value*>> param, Error& outError);


};

