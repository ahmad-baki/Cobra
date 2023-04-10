#include "StdDef.h"
#include "Interpreter.h"

std::map<std::string, cobrfunc_t> StdDef::getFuncs()
{
	return {
		{"input", input},
		{"print", print},
		{"printLi", printLi}
	};
}

std::vector<Value*> StdDef::input(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	if (param[0].size() != 1) {
		return printLi(param, outError);
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { stringTypeId }, outError)) {
		return {};
	}

	std::cout << param[0][0]->toString();
	std::string input_s;
	std::getline(std::cin, input_s);

	std::string* input_h = new std::string(input_s);
	return { new PrimValue(stringTypeId, input_h, outError) };
}

std::vector<Value*> StdDef::print(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	if (param[0].size() != 1) {
		return printLi(param, outError);
	}

	int stringType = Interpreter::getSingelton()->getTypeId("string", outError);
	//if (!PackageManager::cast(param, { stringType }, outError)) {
	//	return {};
	//}

	std::cout << param[0][0]->toString() << std::endl;
	return {};
}

std::vector<Value*> StdDef::printLi(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringType = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { stringType }, outError)) {
		return {};
	}

	std::cout << '[';
	for (size_t i = 0; i < param[0].size(); i++) {
		std::cout << param[0][i]->toString();
		if (i != param[0].size() - 1) {
			std::cout << ", ";
		}
	}
	std::cout << ']' << std::endl;
	return {};
}
