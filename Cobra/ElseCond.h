#pragma once
#include "Statement.h"
#include "Expression.h"
#include "BlockNode.h"

class ElseCond
{
private:
	Expression<bool>* cond;
	Statement* statement;

public:
	ElseCond(Expression<bool>* cond, Statement* statement);
	bool run();
};

