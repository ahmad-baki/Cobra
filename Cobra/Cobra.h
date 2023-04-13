#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]);
std::string trim(std::string input);
std::string readFileIntoString(fs::path path, Error& outError);
double execFromFile(fs::path path, Error& outError);
void execFromCommandLine(Error& outError);
// { { path, fileStr }, ... } 
std::vector<Token> getTokenStream(std::string_view input, fs::path path, Error& outError,
	std::vector<fs::path>& importedFiles);
std::vector<std::tuple<std::string, std::string>> runFileImport(std::vector<Token>& tokens,
	std::vector<fs::path> ignoreFiles, Error& outError);
std::vector<Token> getSTDImports(std::vector<Token>& tokens, std::vector<std::string>& ignorePackages, Error& outError);