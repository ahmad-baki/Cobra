#include "DeclVar.h"
#include "RuntimeError.h"
#include "SyntaxError.h"
#include "Interpreter.h"
#include <format>


DeclVar::DeclVar(std::string name, size_t line, size_t startColumn, 
	size_t endColumn, bool isConst, bool isStaticType, int typeId, bool isList, 
	Expression* value)
	: name{name}, isConst{isConst}, isStaticType{ isStaticType }, 
	typeId{ typeId }, isList{ isList }, value{value}
{
	this->line			= line;
	this->startColumn	= startColumn;
	this->endColumn		= endColumn;

#pragma region OLD-CODE

	//this->line{line};
	//this->startColumn{startColumn};
	//this->endColumn{endColumn};
	//return { {},
	//	RuntimeError("invalid type", line, startColumn, endColumn)
	//};
#pragma endregion 
}

size_t DeclVar::run(Error& outError)
{
	//bool isList;
	//if (sizeExpr == nullptr) {
	//	isList = 1;
	//}
	//else {
	//	std::vector<Value*> values = sizeExpr->run(outError);
	//	if (outError.errorName != "NULL") {
	//		return;
	//	}
	//	if (values.size() != 1) {
	//		RuntimeError targetError{ std::format("Cannot convert from list with size {} to int", 
	//			std::to_string(values.size())) };
	//		outError.copy(targetError);
	//		return;
	//	}
	//	isList = Variable::getIndex(values[0], outError);
	//	if (outError.errorName != "NULL") {
	//		return;
	//	}
	//}

	if (value != nullptr) {
		std::vector<Value*> val = value->run(outError);
		if (outError.errorName != "NULL") {
			outError.line = line;
			outError.startColumn = startColumn;
			outError.endColumn = endColumn;
			return 0;
		}
		Interpreter::getSingelton()->declareVar(name, val, this->typeId, 
			outError, isConst, isStaticType, isList);
	}
	else {
		Interpreter::getSingelton()->declareVar(name, this->typeId,
			outError, isConst, isStaticType, isList);
	}
	if (outError.errorName != "NULL") {
		outError.line			= line;
		outError.startColumn	= startColumn;
		outError.endColumn		= endColumn;
		return 0;
	}
}

#pragma region OLD-CODE
//template<>
//DeclVar<bool>::DeclVar(std::string name, SymbTable* table, 
//	size_t line, size_t startColumn, size_t endColumn)
//	: name{ name }
//{
//	Init(name, 0, table, line, startColumn, endColumn);
//}
//
//template<>
//DeclVar<int>::DeclVar(std::string name, SymbTable* table,
//	size_t line, size_t startColumn, size_t endColumn)
//	: name{ name }
//{
//	Init(name, 0, table, line, startColumn, endColumn);
//}
//
//template<>
//DeclVar<float>::DeclVar(std::string name, SymbTable* table,
//	size_t line, size_t startColumn, size_t endColumn)
//	: name{ name }
//{
//	Init(name, 1, table, line, startColumn, endColumn);
//}

//template<SuppType T>
//void DeclVar<T>::Init(std::string name, int type, SymbTable* table, 
//	size_t line, size_t startColumn, size_t endColumn)
//{
	//this->table = table;
	//this->line = line;
	//this->startColumn = startColumn;
	//this->endColumn;
	//Error error = table->reg(name, type);
	//if (error.m_errorName != "NULL")
	//	return SyntaxError(error.desc, line, startColumn, endColumn);
	//return Error();
//}

//template class DeclVar<bool>;
//template class DeclVar<int>;
//template class DeclVar<float>;
#pragma endregion 
