#pragma once
#include "Statement.h"
#include "Expression.h"
#include "BlockNode.h"
#include "Error.h"

class Loop : public Statement
{
	Expression<bool>* cond;
	Statement* statement;

public:
	Loop(Expression<bool>* cond, Statement* statement);
	Error run();
};
