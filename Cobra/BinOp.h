#pragma once
#include "Expression.h"
#include "TokenType.h"
#include "SuppType.h"


//enum class Op { ADD, SUB, DIV, MUL };

template<SuppType T1, SuppType T2, SuppType T3>
class BinOp : public Expression<T3>
{
private:
	Expression<T1>* expr1;
	Expression<T2>* expr2;
	enum TokenType op;

public:
	BinOp(Expression<T1>* expr1, Expression<T2>* expr2, enum TokenType op);
	T3 run();
};
