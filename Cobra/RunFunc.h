#include "Expression.h"

class RunFunc : public Expression {
public:
	RunFunc(std::string funcName, std::vector<Expression*> params);
	std::vector<Value*> run(Error& outError);

private:
	std::string funcName;
	std::vector<Expression*> params;
};