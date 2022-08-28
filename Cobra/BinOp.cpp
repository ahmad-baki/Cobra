#include "BinOp.h"
#include <stdexcept>
#include "TokenType.CPP"
#include "RuntimeError.h"


template<SuppType T1, SuppType T2, SuppType T3>
inline BinOp<T1, T2, T3>::BinOp(Expression<T1>* expr1, Expression<T2>* expr2, enum TokenType op):
	expr1{ expr1 }, expr2{ expr2 }, op{op} {}

template<SuppType T1, SuppType T2, SuppType T3>
std::pair<T3, Error> BinOp<T1, T2, T3>::run()
{
	auto [val1, val1Error] = expr1->run();

	if (val1Error.m_errorName != "NULL")
		return { {}, val1Error };

	auto [val2, val2Error] = expr2->run();

	if (val2Error.m_errorName != "NULL")
		return { {}, val2Error };


	switch (op)
	{
	case TokenType::PLUS:
		return { T3(val1 + val2), Error() };
	case TokenType::MINUS:
		return { T3(val1 - val2), Error() };
	case TokenType::DIV:
		return { T3(val1 / val2), Error() };
	case TokenType::MUL:
		return { T3(val1 * val2), Error() };
	case TokenType::EQEQ:
		return { T3(val1 == val2), Error() };
	case TokenType::EXCLAEQ:
		return { T3(val1 != val2), Error() };
	case TokenType::SMALL:
		return { T3(val1 < val2), Error() };
	case TokenType::SMALLEQ:
		return { T3(val1 <= val2), Error() };
	case TokenType::BIG:
		return { T3(val1 > val2), Error() };
	case TokenType::BIGEQ:
		return { T3(val1 >= val2), Error() };
	default:
		//return { {}, RuntimeError("invalid operator") };
		throw std::invalid_argument("invalid operator");
	}
}


template class BinOp<bool, bool, bool>;
template class BinOp<bool, int, int>;
template class BinOp<bool, float, float>;
template class BinOp<int, bool, int>;
template class BinOp<int, int, int>;
template class BinOp<int, float, float>;
template class BinOp<float, bool, float>;
template class BinOp<float, int, float>;
template class BinOp<float, float, float>;


template class BinOp<bool, int, bool>;
template class BinOp<bool, float, bool>;
template class BinOp<int, bool, bool>;
template class BinOp<int, int, bool>;
template class BinOp<int, float, bool>;
template class BinOp<float, bool, bool>;
template class BinOp<float, int, bool>;
template class BinOp<float, float, bool>;
