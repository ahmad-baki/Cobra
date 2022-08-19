#include "ElseCond.h"

ElseCond::ElseCond(Expression<bool>* cond, Statement* statement)
	: cond{cond}, statement{ statement } {}

bool ElseCond::run()
{
	if (cond->run()) {
		statement->run();
		return true;
	}
	return false;
}
