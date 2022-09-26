#pragma once
#include <concepts>
#include "Error.h"
#include "Node.h"
class Value;

//template<SuppType T>
class Expression : public Node
{
public:
	virtual std::pair<Value*, Error> run() = 0;
};

