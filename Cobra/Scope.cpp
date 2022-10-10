#include "Scope.h"
#include "RuntimeError.h"
#include <format>
#include "Interpreter.h"
#include <random>

Scope::Scope(std::string name, Error& outError, Value* thisRef)
	: name{name}, variables { std::map<std::string, Value*>() }
{
	 //to stop user from using "this" as a variable
	std::pair<std::string, Value*> elem{ "this", new Value(thisRef, outError, 0, true)};
	variables.emplace(elem);
}

void Scope::exit()
{
	for (auto elem : variables) {
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

bool Scope::declareVar(std::string name, int typeId, Error& outError, void* data, bool isConst, bool isStaticType) {
	if (isVarDecl(name)) {
		RuntimeError targetError = RuntimeError(
			"Tried to declar variable" + name + ", despite it already existing");
		outError.copy(targetError);
		return false;
	}

	Value* newVal = new Value{ typeId, data, outError, isConst, isStaticType };
	variables.emplace(name, newVal);
	return true;
}

bool Scope::setVar(std::string name, Value* tVal, Error& outError)
{
	Value* val = this->getVar(name, outError);

	if (val == nullptr) {
		Error targetError = RuntimeError("Tried to set variable: " + name + ", despite it not existing");
		outError.copy(targetError);
		return false;
	}

	val->setVal(tVal, outError);
	if (outError.errorName != "NULL")
		return false;
	return true;
}

Value* Scope::getVar(std::string name, Error& outError)
{
	auto elem = variables.find(name);
	if (elem == variables.end()) {

		RuntimeError targetError = RuntimeError(
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

bool Scope::declareType(Type* type, Error& outError) {
	if (typeNameReg.contains(type->getTypeName())) {
		RuntimeError targetError{ 
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