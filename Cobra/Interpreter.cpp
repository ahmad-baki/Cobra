#include "Interpreter.h"
#include "RuntimeError.h"
#include <format>
#include <iterator>

Interpreter* Interpreter::singelton = nullptr;

Interpreter::Interpreter()
	: statements{}, scopes{}, nextExecIndex{0}
{
	Error outError{};
	scopes.push_back(Scope("global", outError));
	//blockNode = new BlockNode({}, "global");
	loadStdTypes();
}

Interpreter::~Interpreter() {
	for (auto scope : scopes) {
		scope.exit();
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
		if (outError.errorName != "NULL")
			return;
	}
}

void Interpreter::contin(Error& outError) {
	while (nextExecIndex < statements.size()) {
		statements[nextExecIndex]->run(outError);
		if (outError.errorName != "NULL")
			return;
		nextExecIndex++;
	}
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
	scopes.push_back({ name, outError, thisRef });
}

void Interpreter::ExitScope()
{
	scopes.back().exit();
	scopes.erase(scopes.end()-1);
}

Scope* Interpreter::getCurrScope()
{
	return &scopes.back();
}

bool Interpreter::declareVar(std::string name, std::vector<Value*> val,
	int typeId, Error& outError, bool isConst, bool isStaticType, int size)
{
	return getCurrScope()->declareVar(name, val, typeId, outError, isConst, isStaticType, size);
}

bool Interpreter::declareVar(std::string name, int typeId,
	Error& outError, bool isConst, bool isStaticType, int size)
{
	return getCurrScope()->declareVar(name, typeId, outError, isConst, isStaticType, size);
}

Variable* Interpreter::getVar(std::string name, Error& outError)
{
	Error getError{};
	for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
		getError = Error();
		Variable* var = scope->getVar(name, getError);
		if (var != nullptr) {
			return var;
		}
	}
	outError.copy(getError);
	return nullptr;
}

bool Interpreter::setVar(std::string name, Value* index, std::vector<Value*> tVal, Error& outError)
{
	for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
		if(scope->isVarDecl(name)) {
			return scope->setVar(name, index, tVal, outError);
		}
	}
	Error targetError = RuntimeError("Tried to set variable: " + name + ", despite it not existing");
	outError.copy(targetError);
	return false;
}

bool Interpreter::isVarDecl(std::string name)
{
	for (auto i = scopes.end() - 1; i != scopes.begin()-1; i--) {
		bool isDecl = i->isVarDecl(name);
		if (isDecl) {
			return true;
		}
	}
	return false;
}

Type* Interpreter::getType(std::string typeName, Error& outError) {
	for (auto scope = scopes.rbegin(); scope != scopes.rend(); scope++) {
		if (scope->isTypeDecl(typeName)) {
			return scope->getType(typeName);
		}
	}

	RuntimeError targetError = RuntimeError(
		std::format("Tried to get Type {}, despite it not existing", typeName));
	outError.copy(targetError);
	return nullptr;
}

Type* Interpreter::getType(int id, Error& outError) {
	for (auto scope = scopes.rbegin(); scope != scopes.rend(); scope++) {
		if (scope->isTypeDecl(id)) {
			return scope->getType(id);
		}
	}

	RuntimeError targetError = RuntimeError(
		std::format("Tried to get the Type with id: {}, despite it not existing", id));
	outError.copy(targetError);
	return nullptr;
}

int Interpreter::getTypeId(std::string typeName, Error& outError) {
	for (auto scope = scopes.rbegin(); scope != scopes.rend(); scope++) {
		if (scope->isTypeDecl(typeName)) {
			return scope->getTypeId(typeName);
		}
	}

	RuntimeError targetError = RuntimeError(
		std::format("Tried to get Type {}, despite it not existing", typeName));
	outError.copy(targetError);
	return 0;
}

void Interpreter::loadStdTypes() {
	std::string elemTypes[]{"null", "int", "float"};
	Error outError{};
	for (size_t i = 0; i < std::size(elemTypes); i++) {
		Type* type = new Type(elemTypes[i], i);
		scopes[0].declareType(type, outError);
	}
}

void Interpreter::reset() {
	if (singelton != nullptr) {
		delete singelton;
	}
	singelton = new Interpreter();
}