#include "ExprState.h"


ExprState::ExprState(Expression* expr)  :
    expr{expr}
{
}

size_t ExprState::run(Error& outError)
{
    std::vector<Value*> val = expr->run(outError);
    for (auto i = val.begin(); i != val.end(); i++) {
        if (*i != nullptr) {
            delete *i;
        }
    }
    return 0;
}
