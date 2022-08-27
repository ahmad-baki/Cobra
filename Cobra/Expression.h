#pragma once
#include <concepts>
#include "SuppType.h"
#include "Error.h"

template<SuppType T>
class Expression
{
public:
	virtual std::pair<T, Error> run() = 0;
};

