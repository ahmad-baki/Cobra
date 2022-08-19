#include "BinOp.h"
#include <stdexcept>
#include "TokenType.CPP"
#include <iostream>

template<SuppType T1, SuppType T2, SuppType T3>
inline BinOp<T1, T2, T3>::BinOp(Expression<T1>* expr1, Expression<T2>* expr2, enum TokenType op):
	expr1{ expr1 }, expr2{ expr2 }, op{op} {}

template<SuppType T1, SuppType T2, SuppType T3>
T3 BinOp<T1, T2, T3>::run()
{
	T1 val1 = expr1->run();
	T2 val2 = expr2->run();

	switch (op)
	{
	case TokenType::PLUS:
		return T3(val1 + val2);
	case TokenType::MINUS:
		return T3(val1 - val2);
	case TokenType::DIV:
		return T3(val1 / val2);
	case TokenType::MUL:
		return T3(val1 * val2);
	default:
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