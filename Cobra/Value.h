#pragma once
#include "Expression.h"
#include "SuppType.h"
#include <map>
#include <concepts>

template<typename T>
concept SuppType = std::integral<T> || std::floating_point<T>;

class Value : public Expression
{
public:

	Value();
	Value(Value* other, Error& outError, int typeId, 
		bool isConst = false, bool isStaticType = false);
	Value(int typeId, void* data, Error& outError,
		bool isConst = false, bool isStaticType = false);
	//Value(Value& old_obj);
	~Value();

	static void* Cast(void* data, int o_typeId,
		int t_typeId, Error& outError);

	Value* run(Error& outError);
	void setVal(Value* val, size_t index, Error& outError);
	void setVal(Value* val, Error& outError);
	bool getBool(Error& outError);
	int getType();
	Value* doOp(Value& other, enum TokenType op, Error& outError);
	void* getData();
	friend std::ostream& operator<<(std::ostream& output, const Value& e);

private:
	int typeId;
	void* data;
	bool isConst;
	bool isStaticType;

	template<SuppType T>
	Value* calcOp(T* val1, T* val2, enum TokenType op, int typeId, Error& outError);
	//Value* calcOp(float* val1, float* val2, enum TokenType op, Error& outError);
};