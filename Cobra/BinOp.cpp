#include <string>
#include "BinOp.h"
#include <stdexcept>
#include <cmath>
#include "TokenType.CPP"
#include "RuntimeError.h"
#include <format>


//template<SuppType T1, SuppType T2, SuppType T3>
BinOp::BinOp(Expression* expr1, Expression* expr2, enum TokenType op):
	expr1{ expr1 }, expr2{ expr2 }, op{op}
{
}

//template<SuppType T1, SuppType T2, SuppType T3>
std::vector<Value*> BinOp::run(Error& outError)
{

	std::vector<Value*> val1 = expr1->run(outError);

	if (outError.errType != ErrorType::NULLERROR) {
		return {};
	}

	std::vector<Value*> val2 = expr2->run(outError);

	if (outError.errType != ErrorType::NULLERROR){
		return {};
	}

	if (val1.size() != val2.size()) {
		Error targetError{ ErrorType::RUNTIMEERROR, std::format("The two values have different sizes: {}, {}",
			std::to_string(val1.size()), std::to_string(val2.size())) };
		outError.copy(targetError);
		return {};
	}
	std::vector<Value*> output{};
	output.reserve(val1.size());

	for (size_t i = 0; i < val1.size(); i++) {
		Value* result = val1[i]->doOp(*val2[i], op, outError);

		if (result == nullptr) {
			return {};
		}
		output.push_back(result);
	}
	return output;
#pragma region OLD-CODE
	//switch (op)
	//{
	//case TokenType::PLUS:
	//	return { T3(val1 + val2), Error() };
	//case TokenType::MINUS:
	//	return { T3(val1 - val2), Error() };
	//case TokenType::DIV:
	//	if (val2 == 0) {
	//		return { {},
	//			RuntimeError("ZeroDivisionError: " + std::to_string(val1) + '/' + '0', this->line, this->startColumn, this->endColumn)
	//		};
	//	}
	//	return { T3(val1 / val2), Error() };
	//case TokenType::MUL:
	//	return { T3(val1 * val2), Error() };
	//case TokenType::EQEQ:
	//	return { T3(val1 == val2), Error() };
	//case TokenType::EXCLAEQ:
	//	return { T3(val1 != val2), Error() };
	//case TokenType::SMALL:
	//	return { T3(val1 < val2), Error() };
	//case TokenType::SMALLEQ:
	//	return { T3(val1 <= val2), Error() };
	//case TokenType::BIG:
	//	return { T3(val1 > val2), Error() };
	//case TokenType::BIGEQ:
	//	return { T3(val1 >= val2), Error() };
	//case TokenType::AND:
	//	return { T3(val1 && val2), Error() };
	//case TokenType::OR:
	//	return { T3(val1 || val2), Error() };
	//case TokenType::MOD:
	//	if (val2 == 0) {
	//		return { {},
	//			RuntimeError("ZeroModError: " + std::to_string(val1) + '/' + '0', this->line, this->startColumn, this->endColumn)
	//		};
	//	}
	//	return { std::fmod(val1, val2), Error() };
	//default:
	//	return { {},
	//		RuntimeError("Invalid Operator: ", this->line, this->startColumn, this->endColumn)
	//	};
	//}
#pragma endregion
}

#pragma region OLD-CODE
//template class BinOp<bool, bool, bool>;
//template class BinOp<bool, int, int>;
//template class BinOp<bool, float, float>;
//template class BinOp<int, bool, int>;
//template class BinOp<int, int, int>;
//template class BinOp<int, float, float>;
//template class BinOp<float, bool, float>;
//template class BinOp<float, int, float>;
//template class BinOp<float, float, float>;
//
//
//template class BinOp<bool, int, bool>;
//template class BinOp<bool, float, bool>;
//template class BinOp<int, bool, bool>;
//template class BinOp<int, int, bool>;
//template class BinOp<int, float, bool>;
//template class BinOp<float, bool, bool>;
//template class BinOp<float, int, bool>;
//template class BinOp<float, float, bool>;
#pragma endregion