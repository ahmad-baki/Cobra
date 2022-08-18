#include "Literal.h"

template<SuppType T>
inline Literal<T>::Literal(T value) 
	:value{value}
{
}


template<SuppType T>
T Literal<T>::run() {
	return value;
}


template class Literal<bool>;
template class Literal<int>;
template class Literal<float>;