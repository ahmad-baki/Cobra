#include "BreakState.h"

BreakState::BreakState(size_t n)
	: n{ n }
{
}

size_t BreakState::run(Error& outError) {
	return n;
}