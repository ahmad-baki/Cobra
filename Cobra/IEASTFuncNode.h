#pragma once
#include "IEASTNode.h"

class IEASTFuncNode :
    public IEASTNode
{
public:
    IEASTFuncNode(fs::path path, std::string_view text);
    std::vector<IEASTNode*> params;
    Expression* getExpr(Error& outError);
};

