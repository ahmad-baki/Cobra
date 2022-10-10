#pragma once
#include "Error.h"
#include <vector>
#include "Statement.h"
#include "BlockNode.h"
#include "Scope.h"

class Interpreter
{
public: 
	static Interpreter* getSingelton();
	void run(Error& outError);
	void contin(Error& outError);
	void addStatements(std::vector<Statement*> statements);
	void addStatement(Statement* statement);
	void setStatements(std::vector<Statement*> statements);
	void EnterScope(Error& outError, Value* thisRef = nullptr, std::string name = "anonymous");
	void ExitScope();
	Scope* getCurrScope();
	bool declareVar(std::string name, Value* val, Error& outError, bool isConst = false, bool isStaticType = false);
	bool declareVar(std::string name, int typeId, Error& outError, void* data = nullptr, 
		bool isConst = false, bool isStaticType = false);
	Value* getVar(std::string name, Error& outError);
	bool setVar(std::string name, Value* val, Error& outError);
	bool isVarDecl(std::string name);

	Type* getType(std::string typeName, Error& outError);
	Type* getType(int typeId, Error& outError);
	int getTypeId(std::string typeName, Error& outError);
	void loadStdTypes();

private:
	std::vector<Scope> scopes;
	//BlockNode* blockNode;
	std::vector<Statement*> statements;
	size_t nextExecIndex;
	Interpreter();
	static Interpreter* singelton;
};
