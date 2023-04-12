#include "StdMath.h"
#include "Interpreter.h"
#include <math.h>  

std::map<std::string, cobrfunc_t> StdMath::getFuncs() {
	return {
		{"abs", cobrAbs},
		{"floor", cobrFloor},
		{"ceil", cobrCeil},
		{"round", cobrRound},

		{"sin", cobrSin},
		{"cos", cobrCos},
		{"tan", cobrTan},
		{"aSin", cobrASin},
		{"aCos", cobrACos},
		{"aTan", cobrATan},

		{"root", cobrRoot},
		{"fac", cobrFac},
		{"clamp", cobrClamp},
		{"pow", cobrPow},
		{"powMod", cobrPowMod},
	};
}

std::vector<Value*> StdMath::cobrAbs(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		if (o_typeId == floatTypeId) {
			float result = abs(*(float*)(*iter)->getData());
			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
		}
		else{
			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return {};
			}
			int result = (int)abs(*copy);
			delete copy;
			out.push_back(new PrimValue(intTypeId, new int{ result }, outError));
		}
	}
	return out;
}

std::vector<Value*> StdMath::cobrFloor(std::vector<std::vector<Value*>> param, Error& outError)
{

	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}
	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		if (o_typeId == floatTypeId) {
			float result = floor(*(float*)(*iter)->getData());
			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
		}
		else {
			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return {};
			}
			int result = (int)floor(*copy);
			delete copy;
			out.push_back(new PrimValue(intTypeId, new int{ result }, outError));
		}
	}
	return out;
}

std::vector<Value*> StdMath::cobrCeil(std::vector<std::vector<Value*>> param, Error& outError)
{

	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		if (o_typeId == floatTypeId) {
			float result = ceil(*(float*)(*iter)->getData());
			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
		}
		else {
			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return {};
			}
			int result = (int)ceil(*copy);
			delete copy;
			out.push_back(new PrimValue(intTypeId, new int{ result }, outError));
		}
	}
	return out;
}

std::vector<Value*> StdMath::cobrRound(std::vector<std::vector<Value*>> param, Error& outError)
{

	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		if (o_typeId == floatTypeId) {
			float result = round(*(float*)(*iter)->getData());
			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
		}
		else {
			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return {};
			}
			int result = (int)round(*copy);
			delete copy;
			out.push_back(new PrimValue(intTypeId, new int{ result }, outError));
		}
	}
	return out;
}


std::vector<Value*> StdMath::cobrSin(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId}, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = sin(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrCos(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = cos(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrTan(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = tan(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrASin(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = asin(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrACos(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = acos(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrATan(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (!PackageManager::cast(param, { floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float result = atan(*(float*)param[0][i]->getData());
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}


std::vector<Value*> StdMath::cobrRoot(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (param[1].size() != 1) {
		Error targetError{ErrorType::RUNTIMEERROR, 
			std::format("2. param must have size 1 but size {} was given", 
				param[1].size())};
		outError.copy(targetError);
		return{};
	}

	if (!PackageManager::cast(param, { floatTypeId, floatTypeId }, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float base = *(float*)param[0][i]->getData();
		float exp = 1 / *(float*)param[1][0]->getData();
		float result = pow(base, exp);
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrFac(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 1, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		if (o_typeId == floatTypeId) {
			float val = *(float*)(*iter)->getData();
			if (val < 0) {
				Error targetError{ ErrorType::RUNTIMEERROR,
					std::format("Factorial is not definied for {}", val) };
				outError.copy(targetError);
				return {};
			}

			float result = tgamma(val+1);
			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
		}
		else {
			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
			if (outError.errType != ErrorType::NULLERROR) {
				return {};
			}

			if (*copy < 0) {
				Error targetError{ ErrorType::RUNTIMEERROR,
					std::format("Factorial is not definied for {}", *copy) };
				outError.copy(targetError);
				return {};
			}

			int* result = new int{ (int)tgamma(*copy + 1)};
			delete copy;
			out.push_back(new PrimValue(intTypeId, result, outError));
		}
	}
	return out;
}

std::vector<Value*> StdMath::cobrClamp(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (param[1].size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("2. param must have size 1 but size {} was given",
				param[1].size()) };
		outError.copy(targetError);
		return{};
	}

	if (param[2].size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("2. param must have size 1 but size {} was given",
				param[2].size()) };
		outError.copy(targetError);
		return{};
	}

	if (!PackageManager::cast(param, { floatTypeId, floatTypeId }, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);

	float* min = (float*)PrimValue::Cast(param[1][0]->getData(), param[1][0]->getTypeId(), floatTypeId, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		return {};
	}
	float* max = (float*)PrimValue::Cast(param[1][0]->getData(), param[1][1]->getTypeId(), floatTypeId, outError);
	if (outError.errType != ErrorType::NULLERROR) {
		return {};
	}

	std::vector<Value*> out{};
	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
		int o_typeId = (*iter)->getTypeId();
		float* copy = (float*)PrimValue::Cast((*iter)->getData(), o_typeId, floatTypeId, outError);
		if (outError.errType != ErrorType::NULLERROR) {
			return {};
		}
		float result = std::clamp(*copy, *min, *max);
		delete copy;
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	delete min;
	delete max;
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrPow(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 2, outError)) {
		return {};
	}

	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);

	if (param[1].size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("2. param must have size 1 but size {} was given",
				param[1].size()) };
		outError.copy(targetError);
		return{};
	}

	if (!PackageManager::cast(param, { floatTypeId, floatTypeId }, outError)) {
		return {};
	}

	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		float base = *(float*)param[0][i]->getData();
		float exp = *(float*)param[1][0]->getData();
		float result = pow(base, exp);
		out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

std::vector<Value*> StdMath::cobrPowMod(std::vector<std::vector<Value*>> param, Error& outError)
{
	if (!PackageManager::hasSize(param, 3, outError)) {
		return {};
	}

	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);

	if (param[1].size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("2. param must have size 1 but size {} was given",
				param[1].size()) };
		outError.copy(targetError);
		return{};
	}
	if (param[2].size() != 1) {
		Error targetError{ ErrorType::RUNTIMEERROR,
			std::format("3. param must have size 1 but size {} was given",
				param[2].size()) };
		outError.copy(targetError);
		return{};
	}

	if (!PackageManager::cast(param, { intTypeId, intTypeId, intTypeId }, outError)) {
		return {};
	}

	int exp = *(int*)param[1][0]->getData();
	int mod = *(int*)param[2][0]->getData();
	std::vector<Value*> out{};
	for (size_t i = 0; i < param[0].size(); i++) {
		int base = *(int*)param[0][i]->getData();
		int result = modpow(base, exp, mod);
		out.push_back(new PrimValue(intTypeId, new int{ result }, outError));
	}
	PackageManager::deleteData(param);
	return out;
}

template <SuppType T>
T StdMath::modpow(T base, T exp, T modulus)
{
	base %= modulus;
	T result = 1;
	while (exp > 0) {
		if (exp & 1) result = (result * base) % modulus;
		base = (base * base) % modulus;
		exp >>= 1;
	}
	return result;
}

//template<SuppType T>
//std::vector<Value*> StdMath::MathFunc(std::vector<std::vector<Value*>> param,
//	std::function<T(T)> func, Error& outError) 
//{
//	if (!PackageManager::hasSize(param, 1, outError)) {
//		return {};
//	}
//
//	if (param[0].size() == 0) {
//		return {};
//	}
//
//	int intTypeId = Interpreter::getSingelton()->getTypeId("int", outError);
//	int floatTypeId = Interpreter::getSingelton()->getTypeId("float", outError);
//
//	std::vector<Value*> out{};
//	for (auto iter = param[0].begin(); iter != param[0].end(); iter++) {
//		int o_typeId = (*iter)->getTypeId();
//		if (o_typeId == floatTypeId) {
//			float result = func(*(float*)(*iter)->getData());
//			out.push_back(new PrimValue(floatTypeId, new float{ result }, outError));
//		}
//		else {
//			int* copy = (int*)PrimValue::Cast((*iter)->getData(), o_typeId, intTypeId, outError);
//			if (outError.errType != ErrorType::NULLERROR) {
//				return {};
//			}
//			int* result = new int{ (int)func(*copy) };
//			out.push_back(new PrimValue(intTypeId, result, outError));
//		}
//	}
//	return out;
//}