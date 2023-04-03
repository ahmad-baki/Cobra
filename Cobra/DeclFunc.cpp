#include "DeclFunc.h"
#include "Interpreter.h"

DeclFunc::DeclFunc(std::string funcName, int typeId, bool isList,
	std::vector<DeclVar*> params, Statement* statement):
	funcName{ funcName }, typeId{typeId}, isList{isList}, params{params},
	statement{ statement }
{

}

size_t DeclFunc::run(Error& outError) {
	Interpreter::getSingelton()->declareFunc(funcName, 
		typeId, params, statement, isList, outError);
	return 0;
}