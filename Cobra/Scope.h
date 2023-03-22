#pragma once
#include "Value.h"
#include "Error.h"
#include <map>
#include <string>
#include "Value.h"
#include "CobrClass.h"
#include "Type.h"
#include "Variable.h"

class Scope {

public:
	Scope(std::string name, Error& outError, Value* thisRef = nullptr);
	void exit();

	bool declareVar(std::string name, int typeId, Error& outError, 
		bool isConst = false, bool isStaticType = false, bool isList = false);
	bool declareVar(std::string name, std::vector<Value*> val, int typeId, 
		Error& outError, bool isConst, bool isStaticType, bool isList);
	Variable* getVar(std::string name, Error& outError);
	bool setVar(std::string name, Value* index, std::vector<Value*> tVal, Error& outError);
	bool isVarDecl(std::string name);

	bool declareType(Type* type, Error& outError);
	Type* getType(std::string typeName);
	Type* getType(int typeId);
	int getTypeId(std::string typeName);
	bool isTypeDecl(std::string typeName);
	bool isTypeDecl(int id);

private:
	std::string name;
	std::map<std::string, Variable*> variables;
	std::map<std::string, int> typeNameReg;
	std::map<int, Type*> typeIdReg;
};