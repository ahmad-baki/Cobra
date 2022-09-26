#pragma once
#include "Expression.h"
#include "SuppType.h"

class Value : public Expression
{
public:
	Value();
	Value(Value* other, bool isConst = false, bool isStaticType = false, int dataType = -1);
	Value(int dataType, void* data, bool isConst = false, bool isStaticType = false);
	Value(Value& old_obj);
	~Value();

	static std::pair<void*, Error> Cast(void* data, int o_dataType, int t_dataType);

	std::pair<Value*, Error> run();
	Error setVal(Value* val);
	std::pair<bool, Error> getBool();
	int getType();
	std::pair<Value*, Error> doOp(Value& other, enum TokenType op);
	void* getData();
	friend std::ostream& operator<<(std::ostream& output, const Value& e);

private:
	int dataType;
	void* data;
	bool isConst;
	bool isStaticType;


	template<SuppType T>
	std::pair<Value*, Error> calcOp(T* val1, T* val2, enum TokenType op);
};

