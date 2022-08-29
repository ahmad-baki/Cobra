#pragma once
#include "Expression.h"
#include <string>
#include "SymbTable.h"
#include "SuppType.h"
#include "Error.h"

template<SuppType T>
class GetVar : public Expression<T>
{
private:
    std::string varName;
    SymbTable* table;

public:
    GetVar(std::string varName, SymbTable* table, size_t line, size_t startColumn, size_t endColumn);
    std::pair<T, Error> run();
};

