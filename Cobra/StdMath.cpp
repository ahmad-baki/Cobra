#include "StdMath.h"

std::map<std::string, cobrfunc_t> StdMath::getFuncs() {
	return {
		{"abs", cobrAbs},
		{"floor", cobrFloor},
		{"veil", cobrCeil},
		{"round", cobrRound},
	};
}

std::vector<Value*> StdMath::cobrAbs(std::vector<std::vector<Value*>> param, Error& outError)
{
	return std::vector<Value*>();
}

std::vector<Value*> StdMath::cobrFloor(std::vector<std::vector<Value*>> param, Error& outError)
{
	return std::vector<Value*>();
}

std::vector<Value*> StdMath::cobrCeil(std::vector<std::vector<Value*>> param, Error& outError)
{
	return std::vector<Value*>();
}

std::vector<Value*> StdMath::cobrRound(std::vector<std::vector<Value*>> param, Error& outError)
{
	return std::vector<Value*>();
}
