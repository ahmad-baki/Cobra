#pragma once
#include "Value.h"
#include <vector>
#include "PrimValue.h"

class Variable
{
public:
	// with size = -1 the size is flexible
	Variable(std::vector<Value*> values, Error& outError, int typeId,
		bool isConst, bool isStaticType, bool isList);
	void exit();
	bool setVar(std::vector<Value*>, Value* index, Error& outError);
	std::vector<Value*> getVal(Value* primVal, Error& outError);
	static int getIndex(Value* primVal, Error& outError);

private:
	std::vector<Value*> values;
	bool isConst;
	bool isStaticType;
	bool isList;

};

