#include "Error.h"
#include <vector>
#include <string>
#include <regex>

class PreProcesser {
public:
	PreProcesser(std::string input, std::string_view path);
	std::vector<std::string> run(Error& outError);

private:
	std::string input;
	std::string_view path;

	std::vector<std::string> getInstr();
	//std::vector<std::string> runCustImports(Error& outError);
	size_t skipWhiteSpace(std::string_view str, size_t pos);
};