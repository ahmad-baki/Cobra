#include "Loop.h"

Loop::Loop(Expression<bool>* cond, Statement* statement) 
	: cond{cond}, statement{ statement } {}

void Loop::run()
{
	while ((*cond).run())
		(*statement).run();
}
