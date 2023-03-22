#pragma once
#include "Statement.h"
#include "SuppType.h"
#include "Expression.h"

class PrintState : public Statement
{
private:
	Expression* param;
public:
	PrintState(Expression* param);
	size_t run(Error& outError);
};

