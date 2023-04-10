#include "Interpreter.h"
#include "RuntimeError.h"
#include <format>
#include <iterator>
#include "PackageManager.h"

Interpreter* Interpreter::singelton = nullptr;

Interpreter::Interpreter()
	: statements{}/*, scopes{}*/, sepScopes{}, nextExecIndex{0}
{
	Error outError{};
	sepScopes.push_back({ Scope("global", outError) });
	//blockNode = new BlockNode({}, "global");
	loadStdTypes();
}

Interpreter::~Interpreter() {
	//for (auto scope : scopes) {
	//	scope.exit();
	//}

	for (auto scopes : sepScopes) {
		for (auto scope : scopes) {
			scope.exit();
		}
	}
}

Interpreter* Interpreter::getSingelton() {
	if (singelton == nullptr) {
		singelton = new Interpreter();
	}
	return singelton;
}

void Interpreter::run(Error& outError) {
	for (Statement* statement : statements) {
		statement->run(outError);
		if (outError.errType != ErrorType::NULLERROR)
			return;
	}
}

void Interpreter::contin(Error& outError) {
	while (nextExecIndex < statements.size()) {
		statements[nextExecIndex]->run(outError);
		if (outError.errType != ErrorType::NULLERROR)
			return;
		nextExecIndex++;
	}
}

void Interpreter::importSTD(std::vector<Token> imports, Error& outError)
{
	stdFuncs = PackageManager::import(imports, outError);
}

void Interpreter::addStatements(std::vector<Statement*> statements) {
	for (Statement* statement : statements) {
		this->statements.push_back(statement);
	}
}

void Interpreter::addStatement(Statement* statement) {
	statements.push_back(statement);
}

void Interpreter::setStatements(std::vector<Statement*> statements) {
	this->statements = statements;
}

void Interpreter::EnterScope(Error& outError, Value* thisRef, std::string name)
{
	sepScopes.back().push_back({name, outError, thisRef});
}

void Interpreter::EnterSepScope(Error& outError, Value* thisRef, std::string name) {
	sepScopes.push_back({ { name, outError, thisRef } });
}

void Interpreter::ExitScope()
{
	sepScopes.back().back().exit();
	if (sepScopes.back().size() == 1) {
		sepScopes.erase(sepScopes.end() - 1);
	}
	else {
		sepScopes.back().erase(sepScopes.back().end() - 1);
	}
}

Scope* Interpreter::getCurrScope()
{
	//if (sepScopes.size() > 0) {
	//	return &sepScopes.back().back();
	//}
	//return &scopes.back();
	return &sepScopes.back().back();
}

bool Interpreter::declareVar(std::string name, std::vector<Value*> val,
	int typeId, Error& outError, bool isConst, bool isStaticType, int size)
{
	return getCurrScope()->declareVar(name, val, typeId, outError, isConst, isStaticType, size);
}

bool Interpreter::declareVar(std::string name, int typeId,
	Error& outError, bool isConst, bool isStaticType, bool isList)
{
	return getCurrScope()->declareVar(name, typeId, outError, isConst, isStaticType, isList);
}

Variable* Interpreter::getVar(std::string name, Error& outError)
{
	//if (sepScopes.size() > 0) {
	//	//if (sepScopes.back().isVarDecl(name)) {
	//		return sepScopes.back().getVar(name, outError);
	//	//}
	//	// global scope
	//	//else {
	//		//return scopes[0].getVar(name, outError);
	//	//}
	//}

	Error getError{};
	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes){
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); ++scope) {
			getError = Error();
			Variable* var = scope->getVar(name, getError);
			if (var != nullptr) {
				return var;
			}
		}
	}
	outError.copy(getError);
	return nullptr;
}

bool Interpreter::setVar(std::string name, Value* index, std::vector<Value*> tVal, Error& outError)
{
	//if (sepScopes.size() > 0) {
	//	//if (sepScopes.back().isVarDecl(name)) {
	//		return sepScopes.back().setVar(name, index, tVal, outError);
	//	//}
	//	//// global scope
	//	//else {
	//	//	return scopes[0].getVar(name, outError);
	//	//}
	//}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); ++scope) {
			if (scope->isVarDecl(name)) {
				return scope->setVar(name, index, tVal, outError);
			}
		}
	}
	Error targetError(ErrorType::RUNTIMEERROR, "Tried to set variable: " + name + ", despite it not existing");
	outError.copy(targetError);
	return false;
}

bool Interpreter::isVarDecl(std::string name)
{
	//if (sepScopes.size() > 0) {
	//	return sepScopes.back().isVarDecl(name) /*|| scopes[0].isVarDecl(name)*/;
	//}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto i = scopes->rbegin(); i != scopes->rend(); ++i) {
			if (i->isVarDecl(name)) {
				return true;
			}
		}
	}
	return false;
}


bool Interpreter::declareFunc(std::string name, int typeId, std::vector<DeclVar*> params,
	Statement* statement, bool isList, Error& outError) {
	//return getCurrScope()->declareFunc(name, typeId, params, statement, isList, outError);
	return sepScopes[0][0].declareFunc(name, typeId, params, statement, isList, outError);
}

std::vector<Value*> Interpreter::callFunc(std::string name, 
	std::vector<std::vector<Value*>> params, Error& outError) {
	
	//if (sepScopes.size() > 0) {
	//	if (sepScopes.back().isFuncDecl(name)) {
	//		return sepScopes.back().callFunc(name, params, outError);
	//	}
	//	// global scope
	//	else {
	//		return scopes[0].callFunc(name, params, outError);
	//	}
	//}
	//else {

	auto stdFunc = stdFuncs.find(name);
	if (stdFunc != stdFuncs.end()) {
		return stdFunc->second(params, outError);
	}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); ++scope) {
			Error runError = Error();
			if (scope->isFuncDecl(name)) {
				return scope->callFunc(name, params, outError);
			}
		}
	}
	//}
	Error targetError(ErrorType::RUNTIMEERROR, 
		std::format("Tried to get function {}, despite it not existing", name));
	outError.copy(targetError);
	return {};
}

bool Interpreter::isFuncDecl(std::string name)
{
	//if (sepScopes.size() > 0) {
	//	return sepScopes.back().isFuncDecl(name) || scopes[0].isFuncDecl(name);
	//}

	if (stdFuncs.contains(name)) {
		return true;
	}
	return sepScopes[0][0].isFuncDecl(name);

	//for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
	//	for (auto i = scopes->end() - 1; i != scopes->begin() - 1; i--) {
	//		if (i->isFuncDecl(name)) {
	//			return true;
	//		}
	//	}
	//}
	//return false;
}


Type* Interpreter::getType(std::string typeName, Error& outError) {

	//if (sepScopes.size() > 0) {
	//	if (sepScopes.back().isTypeDecl(typeName)) {
	//		return sepScopes.back().getType(typeName);
	//	}
	//	// global scope
	//	else {
	//		return scopes[0].getType(typeName);
	//	}
	//}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); scope++) {
			if (scope->isTypeDecl(typeName)) {
				return scope->getType(typeName);
			}
		}
	}

	Error targetError(ErrorType::RUNTIMEERROR, 
		std::format("Tried to get Type {}, despite it not existing", typeName));
	outError.copy(targetError);
	return nullptr;
}

Type* Interpreter::getType(int id, Error& outError) {

	//if (sepScopes.size() > 0) {
	//	if (sepScopes.back().isTypeDecl(id)) {
	//		return sepScopes.back().getType(id);
	//	}
	//	// global scope
	//	else {
	//		return scopes[0].getType(id);
	//	}
	//}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); scope++) {
			if (scope->isTypeDecl(id)) {
				return scope->getType(id);
			}
		}
	}

	Error targetError(ErrorType::RUNTIMEERROR, 
		std::format("Tried to get the Type with id: {}, despite it not existing", id));
	outError.copy(targetError);
	return nullptr;
}

int Interpreter::getTypeId(std::string typeName, Error& outError) {

	//if (sepScopes.size() > 0) {
	//	if (sepScopes.back().isTypeDecl(typeName)) {
	//		return sepScopes.back().getTypeId(typeName);
	//	}
	//	// global scope
	//	else {
	//		return scopes[0].getTypeId(typeName);
	//	}
	//}

	for (auto scopes = sepScopes.rbegin(); scopes != sepScopes.rend(); ++scopes) {
		for (auto scope = scopes->rbegin(); scope != scopes->rend(); scope++) {
			if (scope->isTypeDecl(typeName)) {
				return scope->getTypeId(typeName);
			}
		}
	}

	Error targetError(ErrorType::RUNTIMEERROR, 
		std::format("Tried to get Type {}, despite it not existing", typeName));
	outError.copy(targetError);
	return 0;
}

void Interpreter::loadStdTypes() {
	std::string elemTypes[]{"null", "bool", "int", "float", "char", "string"};
	Error outError{};
	for (size_t i = 0; i < std::size(elemTypes); i++) {
		Type* type = new Type(elemTypes[i], i);
		sepScopes[0][0].declareType(type, outError);
	}
}

void Interpreter::reset() {
	if (singelton != nullptr) {
		delete singelton;
	}
	singelton = new Interpreter();
}