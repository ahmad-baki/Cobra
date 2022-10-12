#pragma once
#include "Expression.h"

class Value : public Expression
{
public:
	Value();
	Value(Value* other, Error& outError, int typeId);
	//Value(int typeId, void* data, Error& outError);
	~Value();

	int getTypeId();
	void* getData();
	virtual std::string toString() = 0;

	virtual Value* run(Error& outError) = 0;
	//virtual void setVal(Value* val, size_t index, Error& outError) = 0;
	//virtual void setVal(Value* val, Error& outError) = 0;
	virtual bool getBool(Error& outError) = 0;
	virtual Value* doOp(Value& other, enum TokenType op, Error& outError) = 0;
	virtual Value* getCopy(int typeId, Error& outError) = 0;

protected:
	void* data;
	int typeId;
};

