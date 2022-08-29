#pragma once
#include <concepts>
#include "Expression.h"
#include "Error.h"

template<SuppType T>
class Literal : public Expression<T>
{
private:
	T value;

public:
	Literal(T value, size_t line, size_t startColumn, size_t endColumn);
	std::pair<T, Error> run();
};
