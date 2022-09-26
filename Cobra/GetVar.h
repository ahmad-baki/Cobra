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
    SymbTable* table;

public:
    GetVar(std::string varName, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
    std::pair<Value*, Error> run();
};

