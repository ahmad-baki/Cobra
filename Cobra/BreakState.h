#include "Statement.h"

class BreakState : public Statement {
public:
	BreakState(size_t n);

	size_t run(Error& outError);

private:
	size_t n;
};