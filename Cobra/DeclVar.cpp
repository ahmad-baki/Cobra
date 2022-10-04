#include "DeclVar.h"
#include "RuntimeError.h"
#include "SyntaxError.h"


DeclVar::DeclVar(std::string name, SymbTable* table, size_t line, size_t startColumn, 
	size_t endColumn, bool isConst, bool isStaticType, enum Value::DataType dataType, Expression* expr)
	: name{name}, isConst{isConst}, isStaticType{ isStaticType }, dataType{ dataType }, expr{ expr }
{
	this->table			= table;
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

Error DeclVar::run()
{
	enum Value::DataType targetType{this->dataType};
	void* data{nullptr};
	if (expr != nullptr) {
		auto [val, valError] = expr->run();
		if (valError.m_errorName != "NULL")
			return valError;
		
		if (targetType == -1) {
			targetType	= val->getType();
			data		= val->getData();
		}

		else if (targetType != val->getType()) {
			auto [castVal, castError] = Value::Cast(val->getData(), val->getType(), targetType);
			if (castError.m_errorName != "NULL")
				return castError;
			data = castVal;
		}
		else {
			data = val->getData();
		}
	}
	Error declError = table->declare(name, targetType, data, isConst, isStaticType);
	if (declError.m_errorName != "NULL")
		return declError;
	return Error();
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
