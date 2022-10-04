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
	Value(Value* other, enum DataType dataType = UNDEFINED, bool isConst = false, bool isStaticType = false, bool isIter = false);
	Value(enum DataType dataType, void* data, bool isConst = false, bool isStaticType = false, bool isIter = false);
	Value(Value& old_obj);
	~Value();

	static std::pair<void*, Error> Cast(void* data, enum DataType o_dataType, enum DataType t_dataType);

	std::pair<Value*, Error> run();
	std::pair<Value*, Error> run(size_t index);
	Error setVal(Value* val, size_t index);
	Error setVal(Value* val);
	std::pair<bool, Error> getBool();
	enum DataType getType();
	std::pair<Value*, Error> doOp(Value& other, enum TokenType op);
	void* getData();
	friend std::ostream& operator<<(std::ostream& output, const Value& e);

	Error constrError;
	std::string dataTypeString[3] = {"undefined", "int", "decimal"};
private:
	enum DataType dataType;
	void* data;
	bool isIter;
	bool isConst;
	bool isStaticType;


	template<SuppType T>
	std::pair<Value*, Error> calcOp(T* val1, T* val2, enum TokenType op);
};

