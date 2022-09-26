//#include "Literal.h"
//
//template<SuppType T>
//inline Literal<T>::Literal(T value, size_t line, size_t startColumn, size_t endColumn)
//	:value{value}
//{
//	this->line = line;
//	this->startColumn = startColumn;
//	this->endColumn = endColumn;
//}
//
//
//template<SuppType T>
//std::pair<T, Error> Literal<T>::run() {
//	return { value, Error() };
//}
//
//
//template class Literal<bool>;
//template class Literal<int>;
//template class Literal<float>;