#pragma once
#include "Expression.h"
#include "SuppType.h"

class Value : public Expression
{
public:
	enum DataType {
		UNDEFINED, INTTYPE, DECTYPE 
	};

	Value();
	Value(Value* other, Error& outError, enum DataType dataType = UNDEFINED, 
		bool isConst = false, bool isStaticType = false);
	Value(enum DataType dataType, void* data, Error& outError, 
		bool isConst = false, bool isStaticType = false);
	Value(Value& old_obj);
	~Value();

	static void* Cast(void* data, enum DataType o_dataType, 
		enum DataType t_dataType, Error& outError);

	Value* run(Error& outError);
	void setVal(Value* val, size_t index, Error& outError);
	void setVal(Value* val, Error& outError);
	bool getBool(Error& outError);
	enum DataType getType();
	Value* doOp(Value& other, enum TokenType op, Error& outError);
	void* getData();
	friend std::ostream& operator<<(std::ostream& output, const Value& e);

	std::string dataTypeString[3] = {"undefined", "int", "decimal"};
private:
	enum DataType dataType;
	void* data;
	bool isConst;
	bool isStaticType;


	template<SuppType T>
	Value* calcOp(T* val1, T* val2, enum TokenType op, Error& outError);
};

