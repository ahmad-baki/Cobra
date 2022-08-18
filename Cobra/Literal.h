#pragma once
#include "Expression.h"
#include <concepts>

template<SuppType T>
class Literal : public Expression<T>
{
private:
	T value;

public:
	Literal(T value);
	T run();
};
