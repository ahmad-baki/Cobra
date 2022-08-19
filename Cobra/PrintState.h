#pragma once
#include "Statement.h"
#include "SuppType.h"
#include "Expression.h"

template<SuppType T>
class PrintState : public Statement
{
private:
	Expression<T>* param;
public:
	PrintState(Expression<T>* param);
	void run();
};

