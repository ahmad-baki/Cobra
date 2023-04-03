#include "Func.h"
#include "Interpreter.h"
#include "RuntimeError.h"
#include <string>
#include <format>

Func::Func(std::string name, std::vector<DeclVar*> declParams,
	Statement* statement, int typeId, bool isList) :
	funcName{ name }, declParams{ declParams }, statement{ statement },
	typeId{typeId}, isList{isList}
{
}

void Func::load(std::vector<std::vector<Value*>> vals, Error& outError) {
	if (vals.size() != declParams.size()) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("Invalid number of arguments, {} were passed, while {} arguments is required", 
			std::to_string(vals.size()), std::to_string(declParams.size())) };
		outError.copy(targetError);
		return;
	}
	
	Interpreter::getSingelton()->EnterSepScope(outError, nullptr, funcName);

	for (DeclVar* declVar : declParams) {
		declVar->run(outError);
		if (outError.errType != ErrorType::NULLERROR) {
			return;
		}
	}
	Interpreter::getSingelton()->declareVar("$return", typeId, outError, false, true, isList);
	
	Interpreter* inter = Interpreter::getSingelton();
	for (size_t i = 0; i < declParams.size(); i++) {
		inter->setVar(declParams[i]->getName(), nullptr, vals[i], outError);
		if (outError.errType != ErrorType::NULLERROR) {
			return;
		}
	}
}

std::vector<Value*> Func::run(Error& outError) {

	statement->run(outError);
	if (outError.errType != ErrorType::NULLERROR)
		return {};

	std::vector<Value*> out;
	if (Interpreter::getSingelton()->isVarDecl("$return")) {
		out = Interpreter::getSingelton()->getVar("$return", outError)->getVal(0, outError);
	}
	else {
		out = {};
	}
	if (!isList && out.size() != 1) {
		Error targetError{ErrorType::RUNTIMEERROR, "Cannot return a list"};
		outError.copy(targetError);
		return {};
	}

	for (size_t i = 0; i < out.size(); i++) {
		out[i] = new PrimValue(out[i], outError, typeId);
	}
	Interpreter::getSingelton()->ExitScope();
	return out;
}