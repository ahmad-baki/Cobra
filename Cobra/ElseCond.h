#pragma once
#include "Statement.h"
#include "Expression.h"
#include "BlockNode.h"
#include "Error.h"

class ElseCond : public Node
{
private:
	Expression* cond;
	Statement* statement;

public:
	ElseCond(Expression* cond, Statement* statement, size_t line, size_t startColumn, size_t endColumn);
	bool run(Error& outerror);
};

