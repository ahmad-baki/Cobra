#pragma once
#include "Statement.h"
#include "Expression.h"
#include "ElseCond.h"
#include "BlockNode.h"
#include <vector>

class IfCond : public Statement
{
private:
	Expression* cond;
	Statement* statement;
	std::vector<ElseCond*> ifElseStates;
	Statement* elseState;

public:
	//IfCond(Expression<bool>& cond, BlockNode& block, std::vector<ElseCond> condElseBlocks = std::vector<ElseCond>());
	IfCond(Expression* cond, Statement* statement, std::vector<ElseCond*> ifElseStates, Statement* elseState);
	void run(Error& outError);
};