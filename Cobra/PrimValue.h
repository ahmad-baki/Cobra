#pragma once
#include "Expression.h"
#include "SuppType.h"
#include <map>
#include <concepts>
#include "Value.h"

template<typename T>
concept SuppType = std::integral<T> || std::floating_point<T>;

class PrimValue : public Value
{
public:

	PrimValue(Value* other, Error& outError, int typeId);
	PrimValue(int typeId, void* data, Error& outError);
	~PrimValue();

	static void* Cast(void* data, int o_typeId,
		int t_typeId, Error& outError);

	std::string toString();
	Value* run(Error& outError);
	//void setVal(Value* val, size_t index, Error& outError);
	//void setVal(Value* val, Error& outError);
	bool getBool(Error& outError);
	int getType();
	Value* doOp(Value& other, enum TokenType op, Error& outError);
	void* getData();
	Value* getCopy(int typeId, Error& outError);

private:
	//int typeId;
	//void* data;

	template<SuppType T>
	PrimValue* calcOp(T* val1, T* val2, enum TokenType op, int typeId, Error& outError);
	//Value* calcOp(float* val1, float* val2, enum TokenType op, Error& outError);
};