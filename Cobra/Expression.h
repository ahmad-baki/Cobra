#pragma once
#include <concepts>
#include "Error.h"
#include "Node.h"
#include <vector>

class Value;

//template<SuppType T>
class Expression : public Node
{
public:
	virtual std::vector<Value*> run(Error& outError) = 0;
};

