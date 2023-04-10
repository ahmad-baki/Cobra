#pragma once
#include <vector>
#include "Value.h"
#include <functional>

class StdPackage
{
public:
	std::map<std::string, std::function<std::vector<Value*>(std::vector<Value*>)>> funcs;
};
