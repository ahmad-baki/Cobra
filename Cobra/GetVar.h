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

public:
    GetVar(std::string varName, size_t line, size_t startColumn, size_t endColumn);
    Value* run(Error& outError);
};

