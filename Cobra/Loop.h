#pragma once
#include "Statement.h"
#include "BlockNode.h"
#include "Error.h"

class Loop : public Statement
{
	Expression* cond;
	Statement* statement;

public:
	Loop(Expression* cond, Statement* statement);
	size_t run(Error& outError);
};
