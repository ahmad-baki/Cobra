#pragma once
#include "Expression.h"
#include <string>
#include "SymbTable.h"
#include "SuppType.h"
#include "Error.h"

class GetVar : public Expression
{
private:
    std::string varName;
    Expression* expr;

public:
    GetVar(std::string varName, Expression* expr, size_t line, size_t startColumn, size_t endColumn);
    std::vector<Value*> run(Error& outError);
};

