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
	Literal(T value);
	std::pair<T, Error> run();
};
