#pragma once
#include "Value.h"

class Variable
{
public:
	Variable(Value* tVal, Error& outError, int typeId = 0, 
		bool isConst = false, bool isStaticType = false);
	void exit();
	bool setVar(Value* tVal, Error& outError);
	Value* getVal();

private:
	Value* value;
	bool isConst;
	bool isStaticType;
};

