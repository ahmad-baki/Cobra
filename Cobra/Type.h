#pragma once
#include <set>
#include "Value.h"
#include <map>
//#include "Scope.h"


class Type
{
public:
	struct Memb;

	Type(std::string name, int typeId = 0, int parentId = 0, 
		std::map<std::string, Memb> members = {});

	bool inherit(int typeId, Error& outError);
	bool unInherit(int tpypeId, Error& outError);
	Value* getMemb(std::string name, Error& outError);
	int getTypeId();
	void setTypeId(int id);
	std::string getTypeName();
	//Scope* getScope();

	struct Memb {
		Value* var;
		enum Acces acces;
	};

	enum Acces {
		privateAcc, protectedAcc, publicAcc
	};

private:
	bool isChild(int typeId);
	int typeId;
	std::string typeName;
	int parentId;
	std::set<int> childrenId;
	std::map<std::string, Memb> members;
};

