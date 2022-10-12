#include "Type.h"
#include "RuntimeError.h"
#include <format>
#include "Interpreter.h"


Type::Type(std::string name, int typeId, int parentId,
	std::map<std::string, Memb> members)
	: typeName{name}, typeId{typeId}, parentId{parentId}, members{members}
{
}

bool Type::inherit(int typeId, Error& outError)
{
	if (childrenId.contains(typeId)) {
		RuntimeError targetError{ std::format(
			"Class with id {} already inherits from class", typeId) };
		outError.copy(targetError);
		return false;
	}
	childrenId.insert(typeId);
	return true;
}

bool Type::unInherit(int typeId, Error& outError)
{
	if (childrenId.contains(typeId)) {
		childrenId.erase(typeId);
		return true;
	}
	RuntimeError targetError{ std::format(
		"Class with id {} already inherits from class", typeId) };
	outError.copy(targetError);
	return false;
}

Value* Type::getMemb(std::string name, Error& outError)
{
	auto elem = members.find(name);
	if (elem == members.end()) {
		RuntimeError targetError{ std::format(
			"{} doesnt have member with name: {}", typeName, name) };
		outError.copy(targetError);
		return nullptr;
	}
	if (elem->second.acces == Acces::publicAcc) {
		return elem->second.var;
	}
	
	Interpreter* interpreter = Interpreter::getSingelton();
	Value* thisRef = interpreter->getCurrScope()->getVar("this", outError)->getVal();
	Type* callType = interpreter->getType(thisRef->getTypeId(), outError);
	if (callType == nullptr) {
		return nullptr;
	}
	std::string callTypename = callType->typeName;

	if (elem->second.acces == Acces::protectedAcc) {
		if (isChild(thisRef->getTypeId())) {
			return elem->second.var;
		}

		RuntimeError targetError{ std::format(
			"{} doesnt inherit from {}, so it cant acces protected member {}", 
			callTypename, typeName, name) 
		};
		outError.copy(targetError);
		return nullptr;
	}
	// private acc
	else{
		if (thisRef->getTypeId() == typeId) {
			return elem->second.var;
		}
		RuntimeError targetError{ std::format(
				"{} cant acces private member {}",
				callTypename, name)
		};
		outError.copy(targetError);
		return nullptr;
	}
}

int Type::getTypeId()
{
	return typeId;
}

void Type::setTypeId(int id)
{
	typeId = id;
}

std::string Type::getTypeName()
{
	return typeName;
}

bool Type::isChild(int typeId)
{
	int parent = -1;
	while (parent != typeId) {
		if (parent == 0) {
			return false;
		}
		Error outError{};
		parent = Interpreter::getSingelton()->getType(parent, outError)->parentId;
	}
	return true;
}


//Scope* Type::getScope()
//{
//	return nullptr;
//}
