#pragma once
#include "Value.h"
#include "PackageManager.h"
#include <functional>
#include <map>
#include "PrimValue.h"


class StdMath
{
public:
	static std::map<std::string, cobrfunc_t> getFuncs();

	static std::vector<Value*> cobrAbs(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrFloor(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrCeil(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrRound(std::vector<std::vector<Value*>> param, Error& outError);
	

	static std::vector<Value*> cobrSin(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrCos(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrTan(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrASin(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrACos(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrATan(std::vector<std::vector<Value*>> param, Error& outError);

	static std::vector<Value*> cobrRoot(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrFac(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrClamp(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrPow(std::vector<std::vector<Value*>> param, Error& outError);
	static std::vector<Value*> cobrPowMod(std::vector<std::vector<Value*>> param, Error& outError);


private:
	template <SuppType T>
	static T modpow(T base, T exp, T modulus);
//
//	template<SuppType T>
//	static std::vector<Value*> MathFunc(std::vector<std::vector<Value*>> param, 
//		std::function<T(T)> func, Error& outError);
};
