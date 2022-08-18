#pragma once
#include <concepts>
#include "SuppType.h"

template<SuppType T>
class Expression
{
public:
	virtual T run() = 0;
};

