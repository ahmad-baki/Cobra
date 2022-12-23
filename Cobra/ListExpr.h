#pragma once
#include "Expression.h"
class ListExpr : public Expression
{
public:
	ListExpr();
	void add(Expression* expr);
	void add(ListExpr* expr);
	std::vector<Value*> run(Error& outError);
	const std::vector<Expression*>& getExpr() const;

private:
	std::vector<Expression*> expressions;
};

