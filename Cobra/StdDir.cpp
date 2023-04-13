#include "StdDir.h"
#include "Interpreter.h"
#include <iostream>
#include <fstream> 
#include <filesystem>
#include <locale>
#include <codecvt>

namespace fs = std::filesystem;

std::map<std::string, cobrfunc_t> StdDir::getFuncs()
{
	return {
		{"createDir", createDir},
		{"writeFile", writeFile},
		{"existPath", existPath},
		{"getElemInDir", getElemInDir},
		{"getFile", getFile},
	};
}

std::vector<Value*> StdDir::createDir(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);

	if (!PackageManager::cast(param, { stringTypeId }, outError)) {
		return {};
	}

	int boolTypeId = Interpreter::getSingelton()->getTypeId("bool", outError);
	std::vector<Value*> out{};
	out.reserve(param[0].size());
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		std::string* path = (std::string*)(*iter)->getData();
		try
		{
			fs::create_directories(*path);
			out.push_back(
				new PrimValue(boolTypeId, new bool{ true }, outError)
			);
		}
		catch (const std::exception& e)
		{
			out.push_back(
				new PrimValue(boolTypeId, new bool{ false }, outError)
			);
		}
	}
	return out;
}

std::vector<Value*> StdDir::writeFile(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	if (param[0].size() != param[1].size()) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("1. and 2. must have same size, but sizes {} and {} were given",
				param[0].size(), param[1].size()) };
		outError.copy(targetError);
		return {};
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);
	if (!PackageManager::cast(param, { stringTypeId, stringTypeId }, outError)) {
		return {};
	}

	for (size_t i = 0; i < param[0].size(); i++) {
		std::string* path = (std::string*)param[0][i]->getData();
		std::string* text = (std::string*)param[1][i]->getData();
		try
		{
			std::ofstream outfile;
			outfile.open(*path);
			outfile << text << std::endl;

			outfile.close();
		}
		catch (const std::exception& e)
		{
			Error targetError{ ErrorType::RUNTIMEERROR, e.what() };
			outError.copy(targetError);
			return {};
		}
	}
	return {};
}

std::vector<Value*> StdDir::existPath(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);

	if (!PackageManager::cast(param, { stringTypeId }, outError)) {
		return {};
	}

	int boolTypeId = Interpreter::getSingelton()->getTypeId("bool", outError);
	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		std::string* path = (std::string*)(*iter)->getData();
		out.push_back(
			new PrimValue(boolTypeId, new bool{ fs::exists(*path) }, outError)
		);
	}
	return out;
}

std::vector<Value*> StdDir::getElemInDir(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);

	if (!PackageManager::cast(param, { stringTypeId }, outError)) {
		return {};
	}

	int boolTypeId = Interpreter::getSingelton()->getTypeId("bool", outError);
	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		std::string* path = (std::string*)(*iter)->getData();

		for (const auto& entry : fs::directory_iterator(*path)) {
			try
			{
				const std::string fileName = entry.path().filename().generic_string();
				PrimValue* tok = new PrimValue(stringTypeId, new std::string{ fileName }, outError);
				out.push_back(tok);
			}
			catch (const std::exception&)
			{
			}
		}
	}
	return out;
}

std::vector<Value*> StdDir::getFile(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int stringTypeId = Interpreter::getSingelton()->getTypeId("string", outError);

	if (!PackageManager::cast(param, { stringTypeId }, outError)) {
		return {};
	}

	int boolTypeId = Interpreter::getSingelton()->getTypeId("bool", outError);
	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		std::string* path = (std::string*)(*iter)->getData();
		std::ifstream f("a.txt"); //taking file as inputstream
		std::string str;
		if (f) {
			std::ostringstream ss;
			ss << f.rdbuf(); // reading data
			str = ss.str();
		}
		Value* val = new PrimValue(stringTypeId, new std::string{ str }, outError);
		out.push_back(val);
	}
	return out;
}
