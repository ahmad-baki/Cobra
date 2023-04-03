#include "BlockNode.h"
#include "DeclVar.h"
#include <vector>
#include "Statement.h"

class DeclFunc : public Statement {

public:
	DeclFunc(std::string funcName, int typeId, bool islist, 
		std::vector<DeclVar*> params, Statement* statement);
	size_t run(Error& outError);

private:
	std::string funcName;
	int typeId;
	bool isList;
	std::vector<DeclVar*> params;
	Statement* statement;
};