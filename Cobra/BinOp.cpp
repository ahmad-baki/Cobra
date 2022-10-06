#include <string>
#include "BinOp.h"
#include <stdexcept>
#include <cmath>
#include "TokenType.CPP"
#include "RuntimeError.h"


//template<SuppType T1, SuppType T2, SuppType T3>
BinOp::BinOp(Expression* expr1, Expression* expr2, enum TokenType op):
	expr1{ expr1 }, expr2{ expr2 }, op{op}
{
}

//template<SuppType T1, SuppType T2, SuppType T3>
Value* BinOp::run(Error& outError)
{
	Value* val1 = expr1->run(outError);

	if (val1 == nullptr)
		return nullptr;

	Value* val2 = expr2->run(outError);

	if (val2 == nullptr)
		return nullptr;

	Value* result = val1->doOp(*val2, op, outError);

	if (result == nullptr) {
		return nullptr;
	}
	return result;
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