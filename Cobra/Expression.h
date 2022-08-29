#pragma once
#include <concepts>
#include "SuppType.h"
#include "Error.h"
#include "Node.h"

template<SuppType T>
class Expression : public Node
{
public:
	virtual std::pair<T, Error> run() = 0;
};

