#include "PreProcesser.h"

PreProcesser::PreProcesser(std::string input, std::string_view path) 
	: input {input}, path{path}
{
}

std::vector<std::string> PreProcesser::run(Error& outError) {
	//std::vector<std::string> instr = getInstr();

	//std::smatch match;
	//for (std::string str_v : instr) {
	//	if (std::regex_match(str_v, match, IMPORTREGEX)) {
	//		runImport(str_v, outError);
	//	}
	//}
		
	return {};
}

std::vector<std::string> PreProcesser::getInstr() {
	return {};
}