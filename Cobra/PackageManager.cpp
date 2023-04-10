#include "PackageManager.h"
#include "StdMath.h"
#include "StdDef.h"

std::map<std::string, cobrfunc_t> PackageManager::import(std::vector<Token> packages, Error& outError)
{
	std::map<std::string, cobrfunc_t> out = StdDef::getFuncs();
	
	for (auto iter = packages.begin(); iter != packages.end(); iter++) {
		std::map<std::string, cobrfunc_t> import;

		if (iter->value == "math") {
			import = StdMath::getFuncs();
		}
		else {
			Error targetError = Error(ErrorType::INVCHRERROR, "There is no package " + iter->value, 
				iter->line, iter->startColumn, iter->endColumn, iter->path, iter->text);
			outError.copy(targetError);
		}

		out.insert(import.begin(), import.end());
	}
	

	return out;
}

bool PackageManager::hasSize(std::vector<std::vector<Value*>> param, size_t size, Error& outError)
{
	if (param.size() != size) {
		Error targetError = Error(ErrorType::RUNTIMEERROR, 
			std::format("There is no Function with {} params", param.size()));
		outError.copy(targetError);
		return false;
	}
	return true;
}

bool PackageManager::cast(std::vector<std::vector<Value*>>& param, std::vector<int> t_types, Error& outError)
{
	for (size_t i = 0; i < param.size(); i++) {
		for (size_t j = 0; j < param[i].size(); j++) {
			Value* copy = param[i][j]->getCopy(t_types[i], outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return false;
			}
			param[i][j] = copy;
		}
	}
	return true;
}
