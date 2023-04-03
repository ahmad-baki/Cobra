#pragma once
#include "Expression.h"
#include "DeclVar.h"
#include <vector>
#include "SetVar.h"
#include "BlockNode.h"

class Func 
{
public:
    Func(std::string name, std::vector<DeclVar*> declParams, 
        Statement* statement, int typeId, bool isList);

    void load(std::vector< std::vector<Value*>> vals, Error& outError);
    std::vector<Value*> run(Error& outError);

private:
    const std::vector<DeclVar*> declParams;
    Statement* statement;
    const std::string funcName;
    int typeId;
    bool isList;
};

