#pragma once
#include <concepts>

template<typename T>
concept SuppType = std::integral<T> || std::floating_point<T>;