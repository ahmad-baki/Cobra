#pragma once
#include "Statement.h"
#include "Expression.h"
#include "BlockNode.h"
#include "Error.h"

class ElseCond
{
private:
	Expression<bool>* cond;
	Statement* statement;

public:
	ElseCond(Expression<bool>* cond, Statement* statement);
	std::pair<bool, Error> run();
};

