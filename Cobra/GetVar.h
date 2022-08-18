#pragma once
#include "Expression.h"
#include <string>
#include "SymbTable.h"
#include "SuppType.h"

template<SuppType T>
class GetVar : public Expression<T>
{
private:
    std::string varName;
    SymbTable* table;

public:
    GetVar(std::string varName, SymbTable* table);
    T run();
};

