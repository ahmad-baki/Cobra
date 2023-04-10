#include "StdDef.h"
#include "Interpreter.h"
#include <regex>

std::map<std::string, cobrfunc_t> StdDef::getFuncs()
{
	return {
		{"input", input},
		{"print", print},
		{"printLi", printLi}, 

		{"toBool", toBool},
		{"toInt", toInt},
		{"toFloat", toFloat},
		{"toChar", toChar},
		{"toString", toString},

		// Lists
		{"elemAt", elemAt},
		{"append", append},
		{"removeAt", removeAt},
		{"slice", slice},

		// Strings
		{"strElemAt", strElemAt},
		{"strAppend", strAppend},
		{"strRemoveAt", strRemoveAt},
		{"strSub", strSub},
		{"strMatch", strMatch},
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


std::vector<Value*> StdDef::toBool(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int boolId = Interpreter::getSingelton()->getTypeId("bool", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		out.push_back((*iter)->getCopy(boolId, outError));
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}
	return out;
}

std::vector<Value*> StdDef::toInt(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int intId = Interpreter::getSingelton()->getTypeId("int", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		out.push_back((*iter)->getCopy(intId, outError));
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}
	return out;
}

std::vector<Value*> StdDef::toFloat(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatId = Interpreter::getSingelton()->getTypeId("float", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		out.push_back((*iter)->getCopy(floatId, outError));
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}
	return out;
}

std::vector<Value*> StdDef::toChar(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int charId = Interpreter::getSingelton()->getTypeId("char", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		out.push_back((*iter)->getCopy(charId, outError));
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}
	return out;
}

std::vector<Value*> StdDef::toString(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringId = Interpreter::getSingelton()->getTypeId("string", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		out.push_back((*iter)->getCopy(stringId, outError));
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
	}
	return out;
}


std::vector<Value*> StdDef::elemAt(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}
	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	if (!PackageManager::cast(param, { 0, intTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	out.reserve(param[1].size());

	for (auto iter = param[1].begin(); iter != param[1].end(); iter++) {
		//int* index_p = (int*)PrimValue::Cast((*iter)->getData(), 
		//	(*iter)->getTypeId(), intTypeId, outError);
		//if (outError.errType != ErrorType::NULLERROR) {
		//	return {};
		//}

		int* index_p = (int*)(*iter)->getData();
		if(*index_p > param[0].size() - 1){
			Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot acces element at index {} \
			of variable with size {}", *index_p, param[0].size()) };
			outError.copy(targetError);
			return {};
		}

		out.push_back(param[0][*index_p]->getCopy(0, outError));
	}
	return out;
}

std::vector<Value*> StdDef::append(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	param[0].insert(param[0].end(), param[1].begin(), param[1].end());
	return param[0];
}

std::vector<Value*> StdDef::removeAt(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	if (param[1].size() != 1) {
		Error targetError = Error(ErrorType::RUNTIMEERROR, "2. Parameter cannot be a list");
		outError.copy(targetError);
		return {};
	}
	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	if (!PackageManager::cast(param, { 0, intTypeId }, outError)) {
		return {};
	}

	int* index_p = (int*)param[1][0]->getData();

	if (*index_p > param[0].size() - 1) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot delete element at index {} \
			of variable with size {}", *index_p, param[0].size()) };
		outError.copy(targetError);
		return {};
	}

	param[0].erase(param[0].begin() + *index_p);
	return param[0];
}

std::vector<Value*> StdDef::slice(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 3, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	if (!PackageManager::cast(param, { 0, intTypeId, intTypeId }, outError)) {
		return {};
	}

	int* index_p = (int*)param[1][0]->getData();
	int* length_p = (int*)param[2][0]->getData();

	if (*index_p + *length_p > param[0].size()) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot acces element at index {} \
			of variable with size {}", *index_p + *length_p-1, param[0].size()) };
		outError.copy(targetError);
		return {};
	}

	return { param[0].begin() + *index_p, param[0].begin() + *index_p + *length_p };
}


std::vector<Value*> StdDef::strElemAt(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int strTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	if (!PackageManager::cast(param, { strTypeId, intTypeId }, outError)) {
		return {};
	}

	if (param[0].size() != 1) {
		Error targetError = Error(ErrorType::RUNTIMEERROR, "1. Parameter cannot be a list");
		outError.copy(targetError);
		return {};
	}

	std::vector<Value*> out{};
	out.reserve(param[1].size());

	int charTypeId = Interpreter::getSingelton()->getTypeId("char", outError);
	std::string* str_p = (std::string*)param[0][0]->getData();
	for (auto iter = param[1].begin(); iter != param[1].end(); iter++) {
		//int* index_p = (int*)PrimValue::Cast((*iter)->getData(), 
		//	(*iter)->getTypeId(), intTypeId, outError);
		//if (outError.errType != ErrorType::NULLERROR) {
		//	return {};
		//}

		int* index_p = (int*)(*iter)->getData();
		if (*index_p > str_p->size() - 1) {
			Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot acces element at index {} \
			of variable with size {}", *index_p, str_p->size()) };
			outError.copy(targetError);
			return {};
		}
		Value* val = new PrimValue(charTypeId, new char{ (*str_p)[*index_p] }, outError);
		out.push_back(val);
	}
	return out;
}

std::vector<Value*> StdDef::strAppend(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int strTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { strTypeId, strTypeId }, outError)) {
		return {};
	}

	std::string* str1_p = (std::string*)param[0][0]->getData();
	std::string* str2_p = (std::string*)param[1][0]->getData();
	std::string* val = new std::string{ *str1_p + *str2_p};
	return { new PrimValue(strTypeId, val, outError) };
}

std::vector<Value*> StdDef::strRemoveAt(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int strTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { strTypeId, intTypeId }, outError)) {
		return {};
	}

	std::string* str_p = new std::string{ *(std::string*)param[0][0]->getData() };
	int* index_p = (int*)param[1][0]->getData();

	if (*index_p > str_p->size() - 1) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot acces element at index {} \
			of variable with size {}", *index_p, str_p->size()) };
		outError.copy(targetError);
		return {};
	}

	str_p->erase(str_p->begin() + *index_p);
	return { new PrimValue(strTypeId, str_p, outError) };
}

std::vector<Value*> StdDef::strSub(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 3, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int strTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { strTypeId, intTypeId, intTypeId }, outError)) {
		return {};
	}

	std::string* str_p = (std::string*)param[0][0]->getData();
	int* index_p = (int*)param[1][0]->getData();
	int* length_p = (int*)param[2][0]->getData();

	if (*index_p + *length_p > str_p->size()) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("IndexOutOfBounds: Cannot acces element at index {} \
			of variable with size {}", *index_p + *length_p - 1, param[0].size()) };
		outError.copy(targetError);
		return {};
	}

	std::string* newStr_p = new std::string{ str_p->substr(*index_p, *length_p) };
	return { new PrimValue(strTypeId, newStr_p, outError)} ;
}

std::vector<Value*> StdDef::strMatch(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int strTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { strTypeId, strTypeId }, outError)) {
		return {};
	}

	std::smatch base_match;
	std::string* str_p = (std::string*)param[0][0]->getData();
	std::string* regex_str_p = (std::string*)param[1][0]->getData();
	std::regex regex{ *regex_str_p };

	bool* val = new bool{ std::regex_match(*str_p, base_match, regex) };

	int boolTypeId = Interpreter::getSingelton()->getTypeId("bool", outError);
	return { new PrimValue(boolTypeId, val, outError) };
}


