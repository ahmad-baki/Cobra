#include "DeclVar.h"
#include "RuntimeError.h"
#include "SyntaxError.h"


template<SuppType T>
DeclVar<T>::DeclVar(std::string name, SymbTable* table, size_t line, size_t startColumn, size_t endColumn)
	: name{name}
{
	//this->line{line};
	//this->startColumn{startColumn};
	//this->endColumn{endColumn};
	return { {},
		RuntimeError("invalid type", line, startColumn, endColumn)
	};
}

template<>
DeclVar<bool>::DeclVar(std::string name, SymbTable* table, 
	size_t line, size_t startColumn, size_t endColumn)
	: name{ name }
{
	Init(name, 0, table, line, startColumn, endColumn);
}

template<>
DeclVar<int>::DeclVar(std::string name, SymbTable* table,
	size_t line, size_t startColumn, size_t endColumn)
	: name{ name }
{
	Init(name, 0, table, line, startColumn, endColumn);
}

template<>
DeclVar<float>::DeclVar(std::string name, SymbTable* table,
	size_t line, size_t startColumn, size_t endColumn)
	: name{ name }
{
	Init(name, 1, table, line, startColumn, endColumn);
}

template<SuppType T>
void DeclVar<T>::Init(std::string name, int type, SymbTable* table, 
	size_t line, size_t startColumn, size_t endColumn)
{
	this->table = table;
	this->line = line;
	this->startColumn = startColumn;
	this->endColumn;
	Error error = table->reg(name, type);
	//if (error.m_errorName != "NULL")
	//	return SyntaxError(error.desc, line, startColumn, endColumn);
	//return Error();
}

template<SuppType T>
inline Error DeclVar<T>::run()
{
	Error error = table->declare<T>(name, 0);
	if(error.m_errorName != "NULL")
		return RuntimeError(error.desc, line, startColumn, endColumn);
	return Error();
}


template class DeclVar<bool>;
template class DeclVar<int>;
template class DeclVar<float>;