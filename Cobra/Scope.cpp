#include "Scope.h"
#include "RuntimeError.h"
#include <format>
#include "Interpreter.h"
#include <random>

Scope::Scope(std::string name, Error& outError, Value* thisRef)
	: name{name}, variables{std::map<std::string, Variable*>()}
{
	 //to stop user from using "this" as a variable
	std::pair<std::string, Variable*> elem{ "this", 
		new Variable((thisRef == nullptr) ? std::vector<Value*>{} : std::vector<Value*>{ thisRef }
	, outError, 0, true, true, false) };
	variables.emplace(elem);
}

void Scope::exit()
{
	for (auto elem : variables) {
		if (elem.second != nullptr) {
			delete elem.second;
		}
	}
	for (auto elem : funcs) {
		if (elem.second != nullptr) {
			delete elem.second;
		}
	}
	for (auto elem : typeIdReg) {
		if (elem.second != nullptr) {
			delete elem.second;
		}
	}
}

bool Scope::declareVar(std::string name, int typeId, Error& outError, 
	bool isConst, bool isStaticType, bool isList) {
	if (isVarDecl(name)) {
		Error targetError(ErrorType::RUNTIMEERROR, 
			"Tried to declar variable " + name + ", despite it already existing");
		outError.copy(targetError);
		return false;
	}

	Variable* newVar = new Variable{ {} , outError, typeId, isConst, isStaticType, isList};
	variables.emplace(name, newVar);
	return true;
}

bool Scope::declareVar(std::string name, std::vector<Value*> val, int typeId, 
	Error& outError, bool isConst, bool isStaticType, bool isList)
{
	if (isVarDecl(name)) {
		Error targetError(ErrorType::RUNTIMEERROR, 
			"Tried to declar variable " + name + ", despite it already existing");
		outError.copy(targetError);
		return false;
	}

	Variable* newVar = new Variable{ val, outError, typeId, isConst, isStaticType, isList };
	variables.emplace(name, newVar);
	return true;
}

bool Scope::setVar(std::string name, Value* index, std::vector<Value*> tVal, Error& outError)
{
	Variable* var= this->getVar(name, outError);

	if (var == nullptr) {
		Error targetError(ErrorType::RUNTIMEERROR, "Tried to set variable: " + name + ", despite it not existing");
		outError.copy(targetError);
		return false;
	}

	var->setVar(tVal, index, outError);
	if (outError.errType != ErrorType::NULLERROR)
		return false;
	return true;
}

Variable* Scope::getVar(std::string name, Error& outError)
{
	auto elem = variables.find(name);
	if (elem == variables.end()) {

		Error targetError(ErrorType::RUNTIMEERROR, 
			std::format("Tried to get variable {}, despite it not existing", name));
		outError.copy(targetError);
		return nullptr;
	}
	return elem->second;
}

bool Scope::isVarDecl(std::string name) {
	auto elem = variables.find(name);
	if (elem != variables.end())
		return true;
	return false;
}


bool Scope::declareFunc(std::string name, int typeId, std::vector<DeclVar*> params,
	Statement* statement, bool isList, Error& outError) {
	Func* func = new Func(name, params, statement, typeId, isList);
	funcs.emplace(name, func);
	return true;
}

std::vector<Value*> Scope::callFunc(std::string name, 
	std::vector<std::vector<Value*>> params, Error& outError) 
{
	//auto func = funcs.find(name);
	//if (!funcs.contains(name)) {

	//	Error targetError(ErrorType::RUNTIMEERROR, 
	//		std::format("Tried to get function {}, despite it not existing", name));
	//	outError.copy(targetError);
	//	return {};
	//}
	Func* func = funcs[name];
	func->load(params, outError);
	return func->run(outError);
}


bool Scope::isFuncDecl(std::string name) {
	return funcs.contains(name);
}


bool Scope::declareType(Type* type, Error& outError) {
	if (typeNameReg.contains(type->getTypeName())) {
		Error targetError{ ErrorType::RUNTIMEERROR, 
			std::format("Type with name: {} already exists", 
			type->getTypeName()) 
		};
		outError.copy(targetError);
		return false;
	}

	while (typeIdReg.contains(type->getTypeId())) {
		int newId = (rand() % INT32_MAX - 1) + 1;
		type->setTypeId(newId);
	}

	typeNameReg.emplace(type->getTypeName(), type->getTypeId());
	typeIdReg.emplace(type->getTypeId(), type);
	return true;
}

Type* Scope::getType(std::string typeName)
{
	return typeIdReg[typeNameReg[typeName]];
}

Type* Scope::getType(int typeId)
{
	return typeIdReg[typeId];
}

int Scope::getTypeId(std::string typeName){
	return typeNameReg[typeName];
}

bool Scope::isTypeDecl(std::string typeName) {
	return typeNameReg.contains(typeName);
}

bool Scope::isTypeDecl(int id) {
	return typeIdReg.contains(id);
}