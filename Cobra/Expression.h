#pragma once
#include <concepts>
#include "Error.h"
#include "Node.h"
class Value;

//template<SuppType T>
class Expression : public Node
{
public:
	virtual Value* run(Error& outError) = 0;
};

