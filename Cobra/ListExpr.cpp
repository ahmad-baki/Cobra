#include "ListExpr.h"

ListExpr::ListExpr() :
	expressions{}
{
}

void ListExpr::add(Expression* expr) {
	expressions.push_back(expr);
}

void ListExpr::add(ListExpr* expr) {
	const std::vector<Expression*>& otherExprs = expr->getExpr();
	expressions.reserve(expressions.size() + otherExprs.size());
	for(Expression* var : otherExprs)
	{
		expressions.push_back(var);
	}
}

std::vector<Value*> ListExpr::run(Error& outError) {
	std::vector<Value*> out{};
	out.reserve(expressions.size());
	for (Expression* expr : expressions) {
		std::vector<Value*> val = expr->run(outError);
		if (outError.errorName != "NULL")
			return {};
		out.push_back(val[0]);
	}
	return out;
}

const std::vector<Expression*>& ListExpr::getExpr() const {
	return expressions;
}