#pragma once
#include "Statement.h"
#include "Expression.h"

class ExprState :
    public Statement
{
public:
    ExprState(Expression* expr);
    size_t run(Error& outError);

private:
    Expression* expr;
};

