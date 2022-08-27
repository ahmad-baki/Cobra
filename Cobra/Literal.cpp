#include "Literal.h"

template<SuppType T>
inline Literal<T>::Literal(T value) 
	:value{value}
{
}


template<SuppType T>
std::pair<T, Error> Literal<T>::run() {
	return { value, Error() };
}


template class Literal<bool>;
template class Literal<int>;
template class Literal<float>;